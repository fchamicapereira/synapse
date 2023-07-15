import sys

from abc import ABC, abstractmethod
from typing import TextIO, Union, Optional, Callable

class Command(ABC):
    def __init__(
        self,
        command: str,
        dir: Optional[str] = None,
        source_bashrc: bool = False,
        log_file: Union[bool, str, TextIO] = False,
    ) -> None:
        super().__init__()

        self.command = command
        
        if dir is not None:
            self.command = f"cd {dir}; {self.command}"

        if source_bashrc:
            self.command = f"source $HOME/.bashrc; {self.command}"
        
        if isinstance(log_file, bool):
            self.log_file = sys.stdout if log_file else None
        elif isinstance(log_file, str):
            self.log_file = open(log_file, "a")
        else:
            self.log_file = log_file

        if self.log_file:
            self.log_file.write(f"command: {command}\n")

    @abstractmethod
    def send(self, data: Union[str, bytes]) -> None:
        pass

    @abstractmethod
    def recv(self, size: int) -> str:
        pass

    @abstractmethod
    def recv_stderr(self, size: int) -> str:
        pass

    @abstractmethod
    def exit_status_ready(self) -> bool:
        pass

    @abstractmethod
    def watch(
        self,
        stop_condition: Optional[Callable[[], bool]] = None,
        keyboard_int: Optional[Callable[[], None]] = None,
        timeout: Optional[float] = None,
        stop_pattern: Optional[str] = None,
        max_match_length: Optional[int] = None,
    ) -> str:
        pass

    @abstractmethod
    def recv_exit_status(self) -> int:
        pass

    @abstractmethod
    def fileno(self) -> int:
        pass

    @abstractmethod
    def run_console_commands(
        self,
        commands: Union[str, list[str]],
        timeout: float = 1.0,
        console_pattern: Optional[str] = None,
    ) -> str:
        pass

    @abstractmethod
    def posix_shell(self) -> None:
        pass