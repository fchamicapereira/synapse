import paramiko
import os
import time

from pathlib import Path
from typing import TextIO, Union

from .host import Host

from .commands.command import Command
from .commands.remote import RemoteCommand

class RemoteHost(Host):
    def __init__(
        self, host: str, nb_retries: int = 0, retry_interval: int = 1
    ) -> None:
        self.host = host
        self.nb_retries = nb_retries
        self.retry_interval = retry_interval

        self._ssh_client = None
    
    def run_command(self, *args, **kwargs) -> Command:
        return RemoteCommand(self.ssh_client, *args, **kwargs)
    
    def remote_file_exists(
        self,
        remote_path: Path,
    ) -> bool:
        return super().remote_file_exists(
            self.host,
            remote_path,
        )

    def upload_file(
        self,
        local_path: Path,
        remote_path: Path,
        overwrite: bool = True,
    ) -> None:
        super().upload_file(
            self.host,
            local_path,
            remote_path,
            overwrite,
        )
    
    @property
    def ssh_client(self):
        if self._ssh_client is not None:
            return self._ssh_client

        # adapted from https://gist.github.com/acdha/6064215
        client = paramiko.SSHClient()
        client._policy = paramiko.WarningPolicy()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

        ssh_config = paramiko.SSHConfig()
        user_config_file = os.path.expanduser("~/.ssh/config")
        if os.path.exists(user_config_file):
            with open(user_config_file) as f:
                ssh_config.parse(f)

        cfg = {"hostname": self.host}

        user_config = ssh_config.lookup(self.host)

        for k in ("hostname", "username", "port"):
            if k in user_config:
                cfg[k] = user_config[k]

        if "user" in user_config:
            cfg["username"] = user_config["user"]

        if "proxycommand" in user_config and user_config["proxycommand"]:
            cfg["sock"] = paramiko.ProxyCommand(user_config["proxycommand"])

        if "identityfile" in user_config:
            cfg["pkey"] = paramiko.RSAKey.from_private_key_file(
                user_config["identityfile"][0]
            )

        trial = 0
        while True:
            trial += 1
            try:
                client.connect(**cfg)
                break
            except KeyboardInterrupt as e:
                raise e
            except Exception as e:
                time.sleep(self.retry_interval)
                if trial > self.nb_retries:
                    raise e

        self._ssh_client = client
        return self._ssh_client

    @ssh_client.deleter
    def ssh_client(self):
        if self._ssh_client is not None:
            self._ssh_client.close()
            del self._ssh_client
        self._ssh_client = None

    def __del__(self):
        del self.ssh_client
