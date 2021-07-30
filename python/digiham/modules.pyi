from pycsdr.modules import Module
from digiham.ambe import Mode


class DcBlock(Module):
    def __init__(self):
        ...


class DstarDecoder(Module):
    def __init__(self):
        ...


class FskDemodulator(Module):
    def __init__(self, samplesPerSymbol: int = 40, invert: bool = False):
        ...


class DigitalVoiceFilter(Module):
    def __init__(self):
        ...


class WideRrcFilter(Module):
    def __init__(self):
        ...


class NarrowRrcFilter(Module):
    def __init__(self):
        ...


class MbeSynthesizer(Module):
    def __init__(self, mode: Mode, server: str = ""):
        ...


class NxdnDecoder(Module):
    def __init__(self):
        ...


class GfskDemodulator(Module):
    def __init__(self, samplesPerSymbol: int = 10):
        ...
