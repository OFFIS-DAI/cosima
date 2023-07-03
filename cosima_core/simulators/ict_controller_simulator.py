"""
ICT Controller simulator for OMNeT++ scenarios.

"""
import mosaik_api

from cosima_core.messages.message_pb2 import InfrastructureMessage, AttackMessage
from cosima_core.util.util_functions import log

from scenario_config import TRAFFIC_CONFIGURATION, INFRASTRUCTURE_CHANGES, ATTACK_CONFIGURATION

META = {'models': {
    'ICT': {
        'public': True,
        'params': [],
        'attrs': ['ctrl_message'],
    },
}, 'type': 'event-based'}


class ICTController(mosaik_api.Simulator):
    def __init__(self):
        super().__init__(META)
        self._infrastructure_change_counter = 0
        self._traffic_counter = 0
        self._attack_counter = 0
        self.eid = None
        self._infrastructure_changes = INFRASTRUCTURE_CHANGES
        for event in self._infrastructure_changes:
            event['event_type'] = 'infrastructure_change'
        self._traffic_configuration = TRAFFIC_CONFIGURATION
        for event in self._traffic_configuration:
            event['event_type'] = 'traffic'
        self._attack_configuration = ATTACK_CONFIGURATION
        for event in self._attack_configuration:
            event['event_type'] = 'attack'
        events = list()
        events.extend(self._infrastructure_changes)
        events.extend(self._traffic_configuration)
        events.extend(self._attack_configuration)
        self._events = sorted(events, key=lambda d: d['start'])
        self._current_time = 0

    def init(self, sid, time_resolution=1., **sim_params):
        self.meta["models"]['ICT']['attrs'].append(f'ict_message')
        self.meta["models"]['ICT']['attrs'].append(f'ctrl_message')
        return self.meta

    def create(self, num, model, **model_params):
        if num > 1:
            raise Exception("Only one entity allowed for ICTController.")
        self.eid = "ict_controller"
        return [{'eid': self.eid, 'type': model}]

    def step(self, time, inputs, max_advance):
        log(f'ICT Controller steps in {time} with {inputs}')
        self._current_time = time
        event_starts = list(dict.fromkeys([event['start'] for event in self._events if event['start'] >= time]))
        if len(event_starts) > 1:
            return event_starts[1]
        return None

    def get_data(self, outputs):
        if len(self._events) == 0:
            return {}
        next_event_time = self._events[0]['start']
        next_events = [event for event in self._events if event['start'] == next_event_time]
        outputs = []
        for event in next_events:
            if event['event_type'] == 'infrastructure_change':
                if event['type'] == 'Disconnect':
                    msg_type = InfrastructureMessage.MsgType.DISCONNECT
                else:
                    msg_type = InfrastructureMessage.MsgType.RECONNECT
                outputs.append({
                    'msg_type': msg_type,
                    'msg_id': f'ICTController{event["type"]}_{self._infrastructure_change_counter}',
                    'sim_time': event['start'],
                    'change_module': event['module']
                })
                self._infrastructure_change_counter += 1
            elif event['event_type'] == 'traffic':
                log(f"ICTController: Send traffic information to OMNeT++ at time {next_event_time}.", "info")
                outputs.append({
                    'msg_id': f'ICTController_Traffic_{self._traffic_counter}',
                    'sim_time': event['start'] + 1,
                    'source': event['source'],
                    'destination': event['destination'],
                    'start': event['start'] + 1,
                    'stop': event['stop'],
                    'interval': event['interval'],
                    'packet_length': event['packet_length']
                })
                self._traffic_counter += 1
            elif event['event_type'] == 'attack':
                log(f"ICTController: Start {event['type']} attack at time {next_event_time}.", "info")
                if event['type'] == 'PacketDrop':
                    msg_type = AttackMessage.MsgType.PACKET_DROP
                elif event['type'] == 'PacketFalsification':
                    msg_type = AttackMessage.MsgType.PACKET_FALSIFICATION
                elif event['type'] == 'PacketDelay':
                    msg_type = AttackMessage.MsgType.PACKET_DELAY
                else:
                    raise ValueError('No message type provided in configuration of ICT controller. '
                                     'Please check your scenario configuration file.')
                outputs.append({
                    'msg_type': msg_type,
                    'msg_id': f'ICTController_Attack_{self._attack_counter}',
                    'sim_time': event['start']+1,
                    'attacked_module': event['attacked_module'],
                    'attack_probability': event['probability']*1.0,
                    'stop': event['stop'],
                })
                self._attack_counter += 1
        data = {self.eid: {f'ict_message': outputs}, 'time': next_event_time + 1}
        self._events = [event for event in self._events if event not in next_events]

        return data
