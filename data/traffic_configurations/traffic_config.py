ddos_traffic_configuration = [{
        'source': f'traffic_device_{n_traffic_device}',
        'destination': 'client0',
        'start': start,
        'stop': 100,
        'interval': 2,
        'packet_length': 1000
    } for start in range(1, 5) for n_traffic_device in range(5)]
