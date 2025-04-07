from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class InitialMessage(_message.Message):
    __slots__ = ("msg_id", "max_advance", "until", "step_size", "logging_level", "max_byte_size_per_msg_group")
    MSG_ID_FIELD_NUMBER: _ClassVar[int]
    MAX_ADVANCE_FIELD_NUMBER: _ClassVar[int]
    UNTIL_FIELD_NUMBER: _ClassVar[int]
    STEP_SIZE_FIELD_NUMBER: _ClassVar[int]
    LOGGING_LEVEL_FIELD_NUMBER: _ClassVar[int]
    MAX_BYTE_SIZE_PER_MSG_GROUP_FIELD_NUMBER: _ClassVar[int]
    msg_id: str
    max_advance: int
    until: int
    step_size: int
    logging_level: str
    max_byte_size_per_msg_group: int
    def __init__(self, msg_id: _Optional[str] = ..., max_advance: _Optional[int] = ..., until: _Optional[int] = ..., step_size: _Optional[int] = ..., logging_level: _Optional[str] = ..., max_byte_size_per_msg_group: _Optional[int] = ...) -> None: ...

class InfoMessage(_message.Message):
    __slots__ = ("msg_id", "max_advance", "sim_time", "sender", "receiver", "size", "content", "creation_time", "is_falsified")
    MSG_ID_FIELD_NUMBER: _ClassVar[int]
    MAX_ADVANCE_FIELD_NUMBER: _ClassVar[int]
    SIM_TIME_FIELD_NUMBER: _ClassVar[int]
    SENDER_FIELD_NUMBER: _ClassVar[int]
    RECEIVER_FIELD_NUMBER: _ClassVar[int]
    SIZE_FIELD_NUMBER: _ClassVar[int]
    CONTENT_FIELD_NUMBER: _ClassVar[int]
    CREATION_TIME_FIELD_NUMBER: _ClassVar[int]
    IS_FALSIFIED_FIELD_NUMBER: _ClassVar[int]
    msg_id: str
    max_advance: int
    sim_time: int
    sender: str
    receiver: str
    size: int
    content: str
    creation_time: int
    is_falsified: bool
    def __init__(self, msg_id: _Optional[str] = ..., max_advance: _Optional[int] = ..., sim_time: _Optional[int] = ..., sender: _Optional[str] = ..., receiver: _Optional[str] = ..., size: _Optional[int] = ..., content: _Optional[str] = ..., creation_time: _Optional[int] = ..., is_falsified: bool = ...) -> None: ...

class SynchronisationMessage(_message.Message):
    __slots__ = ("msg_type", "msg_id", "sim_time", "max_advance", "timeout", "timeout_msg_id")
    class MsgType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        MAX_ADVANCE: _ClassVar[SynchronisationMessage.MsgType]
        WAITING: _ClassVar[SynchronisationMessage.MsgType]
        TRANSMISSION_ERROR: _ClassVar[SynchronisationMessage.MsgType]
    MAX_ADVANCE: SynchronisationMessage.MsgType
    WAITING: SynchronisationMessage.MsgType
    TRANSMISSION_ERROR: SynchronisationMessage.MsgType
    MSG_TYPE_FIELD_NUMBER: _ClassVar[int]
    MSG_ID_FIELD_NUMBER: _ClassVar[int]
    SIM_TIME_FIELD_NUMBER: _ClassVar[int]
    MAX_ADVANCE_FIELD_NUMBER: _ClassVar[int]
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    TIMEOUT_MSG_ID_FIELD_NUMBER: _ClassVar[int]
    msg_type: SynchronisationMessage.MsgType
    msg_id: str
    sim_time: int
    max_advance: int
    timeout: bool
    timeout_msg_id: str
    def __init__(self, msg_type: _Optional[_Union[SynchronisationMessage.MsgType, str]] = ..., msg_id: _Optional[str] = ..., sim_time: _Optional[int] = ..., max_advance: _Optional[int] = ..., timeout: bool = ..., timeout_msg_id: _Optional[str] = ...) -> None: ...

class InfrastructureMessage(_message.Message):
    __slots__ = ("msg_type", "msg_id", "sim_time", "change_module", "connection_change_successful")
    class MsgType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        DISCONNECT: _ClassVar[InfrastructureMessage.MsgType]
        RECONNECT: _ClassVar[InfrastructureMessage.MsgType]
    DISCONNECT: InfrastructureMessage.MsgType
    RECONNECT: InfrastructureMessage.MsgType
    MSG_TYPE_FIELD_NUMBER: _ClassVar[int]
    MSG_ID_FIELD_NUMBER: _ClassVar[int]
    SIM_TIME_FIELD_NUMBER: _ClassVar[int]
    CHANGE_MODULE_FIELD_NUMBER: _ClassVar[int]
    CONNECTION_CHANGE_SUCCESSFUL_FIELD_NUMBER: _ClassVar[int]
    msg_type: InfrastructureMessage.MsgType
    msg_id: str
    sim_time: int
    change_module: str
    connection_change_successful: bool
    def __init__(self, msg_type: _Optional[_Union[InfrastructureMessage.MsgType, str]] = ..., msg_id: _Optional[str] = ..., sim_time: _Optional[int] = ..., change_module: _Optional[str] = ..., connection_change_successful: bool = ...) -> None: ...

class TrafficMessage(_message.Message):
    __slots__ = ("msg_id", "sim_time", "source", "destination", "start", "stop", "interval", "packet_length")
    MSG_ID_FIELD_NUMBER: _ClassVar[int]
    SIM_TIME_FIELD_NUMBER: _ClassVar[int]
    SOURCE_FIELD_NUMBER: _ClassVar[int]
    DESTINATION_FIELD_NUMBER: _ClassVar[int]
    START_FIELD_NUMBER: _ClassVar[int]
    STOP_FIELD_NUMBER: _ClassVar[int]
    INTERVAL_FIELD_NUMBER: _ClassVar[int]
    PACKET_LENGTH_FIELD_NUMBER: _ClassVar[int]
    msg_id: str
    sim_time: int
    source: str
    destination: str
    start: int
    stop: int
    interval: int
    packet_length: int
    def __init__(self, msg_id: _Optional[str] = ..., sim_time: _Optional[int] = ..., source: _Optional[str] = ..., destination: _Optional[str] = ..., start: _Optional[int] = ..., stop: _Optional[int] = ..., interval: _Optional[int] = ..., packet_length: _Optional[int] = ...) -> None: ...

class AttackMessage(_message.Message):
    __slots__ = ("msg_type", "msg_id", "sim_time", "attacked_module", "start", "stop", "attack_probability")
    class MsgType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        PACKET_DROP: _ClassVar[AttackMessage.MsgType]
        PACKET_FALSIFICATION: _ClassVar[AttackMessage.MsgType]
        PACKET_DELAY: _ClassVar[AttackMessage.MsgType]
    PACKET_DROP: AttackMessage.MsgType
    PACKET_FALSIFICATION: AttackMessage.MsgType
    PACKET_DELAY: AttackMessage.MsgType
    MSG_TYPE_FIELD_NUMBER: _ClassVar[int]
    MSG_ID_FIELD_NUMBER: _ClassVar[int]
    SIM_TIME_FIELD_NUMBER: _ClassVar[int]
    ATTACKED_MODULE_FIELD_NUMBER: _ClassVar[int]
    START_FIELD_NUMBER: _ClassVar[int]
    STOP_FIELD_NUMBER: _ClassVar[int]
    ATTACK_PROBABILITY_FIELD_NUMBER: _ClassVar[int]
    msg_type: AttackMessage.MsgType
    msg_id: str
    sim_time: int
    attacked_module: str
    start: int
    stop: int
    attack_probability: int
    def __init__(self, msg_type: _Optional[_Union[AttackMessage.MsgType, str]] = ..., msg_id: _Optional[str] = ..., sim_time: _Optional[int] = ..., attacked_module: _Optional[str] = ..., start: _Optional[int] = ..., stop: _Optional[int] = ..., attack_probability: _Optional[int] = ...) -> None: ...

class CosimaMsgGroup(_message.Message):
    __slots__ = ("initial_messages", "info_messages", "synchronisation_messages", "infrastructure_messages", "traffic_messages", "attack_messages", "current_time_step", "number_of_message_groups", "number_of_messages")
    INITIAL_MESSAGES_FIELD_NUMBER: _ClassVar[int]
    INFO_MESSAGES_FIELD_NUMBER: _ClassVar[int]
    SYNCHRONISATION_MESSAGES_FIELD_NUMBER: _ClassVar[int]
    INFRASTRUCTURE_MESSAGES_FIELD_NUMBER: _ClassVar[int]
    TRAFFIC_MESSAGES_FIELD_NUMBER: _ClassVar[int]
    ATTACK_MESSAGES_FIELD_NUMBER: _ClassVar[int]
    CURRENT_TIME_STEP_FIELD_NUMBER: _ClassVar[int]
    NUMBER_OF_MESSAGE_GROUPS_FIELD_NUMBER: _ClassVar[int]
    NUMBER_OF_MESSAGES_FIELD_NUMBER: _ClassVar[int]
    initial_messages: _containers.RepeatedCompositeFieldContainer[InitialMessage]
    info_messages: _containers.RepeatedCompositeFieldContainer[InfoMessage]
    synchronisation_messages: _containers.RepeatedCompositeFieldContainer[SynchronisationMessage]
    infrastructure_messages: _containers.RepeatedCompositeFieldContainer[InfrastructureMessage]
    traffic_messages: _containers.RepeatedCompositeFieldContainer[TrafficMessage]
    attack_messages: _containers.RepeatedCompositeFieldContainer[AttackMessage]
    current_time_step: int
    number_of_message_groups: int
    number_of_messages: int
    def __init__(self, initial_messages: _Optional[_Iterable[_Union[InitialMessage, _Mapping]]] = ..., info_messages: _Optional[_Iterable[_Union[InfoMessage, _Mapping]]] = ..., synchronisation_messages: _Optional[_Iterable[_Union[SynchronisationMessage, _Mapping]]] = ..., infrastructure_messages: _Optional[_Iterable[_Union[InfrastructureMessage, _Mapping]]] = ..., traffic_messages: _Optional[_Iterable[_Union[TrafficMessage, _Mapping]]] = ..., attack_messages: _Optional[_Iterable[_Union[AttackMessage, _Mapping]]] = ..., current_time_step: _Optional[int] = ..., number_of_message_groups: _Optional[int] = ..., number_of_messages: _Optional[int] = ...) -> None: ...
