import jinja2
from numpy import ceil

with open("tuse_cb_mkrecv.template","r") as f:
    tuse_cb_mkrecv_template = jinja2.Template(f.read())

with open("tuse_ib_mkrecv.template","r") as f:
    tuse_ib_mkrecv_template = jinja2.Template(f.read())

with open("fbfuse_cb_mksend.template","r") as f:
    fbfuse_cb_mksend_template = jinja2.Template(f.read())

with open("fbfuse_ib_mksend.template","r") as f:
    fbfuse_ib_mksend_template = jinja2.Template(f.read())


def generate_test_mkrecv_cb_config(nantennas, beam_ids, freq_ids, mcast_groups,
    interface, nchans=4096, dada_key=0xdada):

    nantennas = 2**(nantennas-1).bit_length()
    nchans_per_heap = nchans / nantennas
    packet_size = 8192 # jumbo ethernet frame
    nsamples_per_packet = packet_size / nchans_per_heap;
    npackets_per_heap = 256
    heap_size = npackets_per_heap * packet_size
    nsamples_per_heap = npackets_per_heap * nsamples_per_packet
    valid_freq_ids = range(0, nchans, nchans_per_heap)

    if not set(freq_ids).issubset(set(valid_freq_ids)):
        raise Exception("Invalid frequency IDs passed")

    # MKRECV config
    rendered = tuse_cb_mkrecv_template.render(
        obs_id='test_obs',                           # source = CAM
        src_name='test_source',                      # source = CAM
        src_ra='00:00:00',                           # source = CAM
        src_dec='00:00:00',                          # source = CAM
        receiver='l-band',                           # source = CAM
        centre_frequency=1420.0e6,                   # source = FBFUSE
        bandwidth=856e6,                             # source = FBFUSE
        integration_time=7.65607476635514e-05,       # source = FBFUSE
        dada_key=dada_key,                           # source = TUSE
        sync_time=0.0,                               # source = CAM
        sample_clock=2*856e6,                        # source = CAM
        mcast_groups_csv=",".join(mcast_groups),     # source = FBFUSE
        interface_address=interface,                 # source = TUSE
        heap_size=heap_size,                         # source = FBFUSE
        nsamples_per_heap=nsamples_per_heap,         # source = FBFUSE
        beam_ids_csv=",".join(map(str,beam_ids)),             # source = TUSE + FBFUSE
        freq_channel_ids_csv=",".join(map(str,freq_ids)),     # source = TUSE + FBFUSE
        )
    return rendered

def generate_test_mksend_cb_config(nantennas, beam_ids, freq_id, mcast_groups,
    interface, worker_number=0, nchans=4096, dada_key=0xcaca):

    nantennas = 2**(nantennas-1).bit_length()
    nchans_per_heap = nchans / nantennas
    packet_size = 8192 # jumbo ethernet frame
    nsamples_per_packet = packet_size / nchans_per_heap;
    npackets_per_heap = 256
    heap_size = npackets_per_heap * packet_size
    nsamples_per_heap = npackets_per_heap * nsamples_per_packet
    freq_ids = range(0, nchans, nchans_per_heap)
    data_rate = 856e6 * (nchans_per_heap/float(nchans)) * 8 / 16
    heap_start = (128 * worker_number) + 1
    heap_offset = 1
    heap_step = 128 * 64
    rendered = fbfuse_cb_mksend_template.render(
        interface_address=interface,
        dada_key=dada_key,
        sync_time=0.0,
        sample_clock=2*856e6,
        mcast_groups_csv=",".join(mcast_groups),
        heap_size=heap_size,
        heap_start=heap_start,
        heap_offset=heap_offset,
        heap_step=heap_step,
        data_rate=data_rate,
        nsamples_per_heap=nsamples_per_heap,
        beam_ids_csv=",".join(map(str,beam_ids)),
        freq_id=freq_id)

    return rendered






