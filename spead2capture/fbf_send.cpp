#include <iostream>
#include <utility>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <spead2/common_endian.h>
#include <spead2/common_thread_pool.h>
#include <spead2/common_defines.h>
#include <spead2/common_flavour.h>
#include <spead2/send_heap.h>
#include <spead2/send_udp.h>
#include <spead2/send_stream.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT "8888"
#define DEFAULT_NBEAMS 6
#define DEFAULT_NHEAPS 100
#define DEFAULT_RATE 0.0
#define DEFAULT_NCHANS_PER_SUBBAND 128
#define DEFAULT_NSUBBANDS 32
#define HEAP_ADDRESS_BITS 48

using boost::asio::ip::udp;

static std::atomic<bool> first_heap(true); 

namespace
{
  const size_t ERROR_IN_COMMAND_LINE = 1;
  const size_t SUCCESS = 0;
  const size_t ERROR_UNHANDLED_EXCEPTION = 2;
} // namespace 

int main(int argc, char** argv)
{

  std::uint32_t nsubbands = DEFAULT_NSUBBANDS;
  std::uint32_t nbeams = DEFAULT_NBEAMS;
  std::uint32_t nheaps = DEFAULT_NHEAPS;
  std::uint32_t nchans_per_subband = DEFAULT_NCHANS_PER_SUBBAND;
  float rate = DEFAULT_RATE; // bits per second
  std::string hostname = DEFAULT_HOST;
  std::string port = DEFAULT_PORT;
  std::string interface = "";	

  
  namespace po = boost::program_options;
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "Print help messages")
    ("nsubbands,s", po::value<std::uint32_t>(&nsubbands)->default_value(DEFAULT_NSUBBANDS),
     "Number of subbands to send per timestep")
    ("nchans,c", po::value<std::uint32_t>(&nchans_per_subband)->default_value(DEFAULT_NCHANS_PER_SUBBAND),
     "The number of frequency channels per subband")
    ("nbeams,b", po::value<std::uint32_t>(&nbeams)->default_value(DEFAULT_NBEAMS),
     "The number of beams in the stream")
    ("nheaps,n", po::value<std::uint32_t>(&nheaps)->default_value(DEFAULT_NHEAPS),
     "The number of heaps to send")
    ("interface,i", po::value<std::string>(&interface)->default_value(""),
     "The interface to bind to")	
    ("rate,r", po::value<float>(&rate)
     ->default_value(DEFAULT_RATE)
     ->notifier([&rate](float _rate){rate=_rate/8.;}),
     "The maximum data rate to send at in Gb/s")
    ("host,H", po::value<std::string>(&hostname)->default_value(DEFAULT_HOST),
     "The host IP to send to")
    ("port,p", po::value<std::string>(&port)->default_value(DEFAULT_PORT),
     "The host port to send to");
  po::variables_map vm;
  try
    {
      po::store(po::parse_command_line(argc, argv, desc), vm);
      if ( vm.count("help")  )
	{
	  std::cout << "fbf_test -- Create one multicast streams worth of FBFUSE coherent beam output"
		    << std::endl << desc << std::endl;
	  return SUCCESS;
	}
      po::notify(vm);
    }
  catch(po::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;
      return ERROR_IN_COMMAND_LINE;
    }
  
  spead2::thread_pool tp;
  udp::resolver resolver(tp.get_io_service());
  udp::resolver::query query(hostname, port);
  auto it = resolver.resolve(query);
  spead2::flavour f(spead2::maximum_version, 64, HEAP_ADDRESS_BITS);

  auto config = spead2::send::stream_config(8192, rate*1024*1024*1024, 65536, 1000);

  std::cout << "Rate is:"<< config.get_rate() << "\n";
  spead2::send::udp_stream* stream_ptr;	

  if (interface.length() != 0)
    {		
      stream_ptr = new spead2::send::udp_stream(tp.get_io_service(), *it, config, 8 * 1024 * 1024, 1, 
        boost::asio::ip::address_v4::from_string(interface));
    }
  else
    {
      stream_ptr = new spead2::send::udp_stream(tp.get_io_service(), *it, config);
    }  
  spead2::send::udp_stream& stream = *stream_ptr;		
  std::vector<float> timings;
  
  spead2::descriptor timestamp_desc;
  timestamp_desc.id = 0x1600;
  timestamp_desc.name = "timestamp";
  timestamp_desc.description = "sampler clock ticks since epoch";
  timestamp_desc.format.emplace_back('u', HEAP_ADDRESS_BITS);

  spead2::descriptor beam_id_desc;
  beam_id_desc.id = 0x1601;
  beam_id_desc.name = "beam_id";
  beam_id_desc.description = "beam ID";
  beam_id_desc.format.emplace_back('u', HEAP_ADDRESS_BITS);

  spead2::descriptor frequency_desc;
  frequency_desc.id = 0x4103;
  frequency_desc.name = "frequency";
  frequency_desc.description = "subband ID";
  frequency_desc.format.emplace_back('u', HEAP_ADDRESS_BITS);

  /*spead2::descriptor nchans_desc;
  nchans_desc.id = 0x1602;
  nchans_desc.name = "nchans";
  nchans_desc.description = "number of channels in subband";
  nchans_desc.format.emplace_back('u', HEAP_ADDRESS_BITS);*/

  spead2::descriptor fbf_raw_desc;
  fbf_raw_desc.id = 0x1603;
  fbf_raw_desc.name = "fbf_raw";
  fbf_raw_desc.description = "beamformed time-frequency data";
  fbf_raw_desc.format.emplace_back('i', 8);

  std::uint64_t timestamp = 0;
  std::uint64_t nchans = nchans_per_subband;
  std::uint64_t nsamples = 64*128;
  unsigned n = 0;
  std::vector<char> fbf_raw(nsamples * nchans, 0);
  std::generate(fbf_raw.begin(),fbf_raw.end(),[&]()
                                              {
                                                  if(n <= 127){
                                                  return ++n;
                                                  }
                                                  else
                                                  { n=0;
                                                    return n;
                                                  }      
                                              });


  for (std::size_t heap=0; heap < nheaps; ++heap)
    {
      auto start = std::chrono::high_resolution_clock::now();
      for (int beam_id=0; beam_id < nbeams; ++beam_id)
	  {
       std::uint64_t frequency = 0;
	   for (std::uint32_t subband=0; subband < nsubbands; ++subband)
	   {
	      spead2::send::heap* h_ptr = new spead2::send::heap(f);

	      if (first_heap)
		{
		  h_ptr->add_descriptor(timestamp_desc);
		  h_ptr->add_descriptor(beam_id_desc);
		  h_ptr->add_descriptor(frequency_desc);
		  //h_ptr->add_descriptor(nchans_desc);
		  h_ptr->add_descriptor(fbf_raw_desc);
		  first_heap = false;
		}
	      frequency = 128*subband;

	      //timestamp
	      h_ptr->add_item(0x1600, timestamp);
	      //beam_id
	      h_ptr->add_item(0x1601, beam_id);
	      //frequency
	      h_ptr->add_item(0x4103, frequency);
	      //nchans
	      //h_ptr->add_item(0x1602, nchans);
	      //fbf_raw
	      h_ptr->add_item(0x1603, fbf_raw.data(), fbf_raw.size(), false);

	      stream.async_send_heap(*h_ptr, [h_ptr, subband, beam_id, timestamp]
				     (const boost::system::error_code &ec, spead2::item_pointer_t bytes_transferred)
				     {
				       if (ec)
					 std::cerr << ec.message() << '\n';
				       
				       else
					 std::cout << "Sent " << bytes_transferred
						   << " bytes in heap (" << subband
						   << ", " << beam_id << ", "
						   << timestamp<<")\n";
				       
				       delete h_ptr;
				     });
	    } //subband loop 
	} //beam loop
      timestamp = timestamp + 2048;
      stream.flush();
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> diff = end-start;
      std::cout << "One timestamp took " << diff.count() << " s\n";
      timings.push_back((float)diff.count());
    } //heap loop

  float avg_timing = std::accumulate(timings.begin(),timings.end(),0.0) / (float) timings.size();
  std::cout << "Average execution time " << avg_timing << " s\n";
  
  spead2::send::heap end(f);
  end.add_end();
  stream.async_send_heap(end, [] (const boost::system::error_code &ec, spead2::item_pointer_t bytes_transferred) {});
  stream.flush();

  delete stream_ptr;
  return 0;
}

