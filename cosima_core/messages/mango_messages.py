from dataclasses import dataclass

import mango.messages.codecs as codecs


@codecs.json_serializable
@dataclass
class AlphabetMessage:
    content: str


@codecs.json_serializable
@dataclass
class SimulatorMessage:
    content: str
