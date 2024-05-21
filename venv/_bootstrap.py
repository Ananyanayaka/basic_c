#! /usr/bin/env python3

'''
Download and install cip_build_system in a virtual environment.

Please modify templates/scripts/bootstrap.py instead of
external_scripts/bootstrap.py since bootstrap.py is overridden by the build
process.
'''

###############################################################################
# START: Common code section for cip.py and bootstrap.py
###############################################################################

import sys
import subprocess
import os
from io import StringIO
from functools import reduce
import argparse

import abc
import hashlib
import http
import json
import os.path
import pathlib
import random
import time
import typing
from urllib.error import URLError
import urllib.request

http.client.HTTPConnection.debuglevel = 5

DEFAULT_DOWNLOAD_RETRIES = 9
MIN_SECONDS_BETWEEN_DOWNLOAD_RETRIES = 3
MAX_SECONDS_BETWEEN_DOWNLOAD_RETRIES = 10

class NetworkAccess(abc.ABC):
    """Abstract interface for network access"""
    def read(self, url: str) -> bytes:
        """Fetch the resource at the given URL and deliver its content as bytes"""

class HttpNetworkAccess(NetworkAccess):
    """HTTP implementation of the network access interface"""
    def read(self, url: str) -> bytes:
        """Fetch the resource at the given URL and deliver its content as bytes"""
        with urllib.request.urlopen(url) as response:
            return response.read()

class FilesystemAccess(abc.ABC):
    """Abstract interface for filesystem access"""
    def read(self, path: pathlib.Path) -> bytes:
        """Read the file at the given path and deliver its content as bytes"""

    def write(self, path: pathlib.Path, content: bytes):
        """Write the binary content to the specified path. The parent 
           directories for "path" are created if needed."""

    def exists(self, path: pathlib.Path) -> bool:
        """Check whether the given path exists"""

    def getmtime(self, path: pathlib.Path) -> float:
        """Return the time of last modification of path. The return value is a 
        floating point number giving the number of seconds since the epoch."""

class PhysicalFilesystemAccess(FilesystemAccess):
    """Physical implementation of the filesystem access interface"""
    def read(self, path: pathlib.Path) -> bytes:
        """Read the file at the given path and deliver its content as bytes"""
        with path.open("rb") as f:
            return f.read()

    def write(self, path: pathlib.Path, content: bytes):
        """Write the binary content to the specified path. The parent 
           directories for "path" are created if needed."""
        parent_dir = path.parent

        if not parent_dir.exists():
            parent_dir.mkdir(parents=True)

        with path.open('wb') as output_stream:
            output_stream.write(content)

    def exists(self, path: pathlib.Path) -> bool:
        """Check whether the given path exists"""
        return path.exists()

    def getmtime(self, path: pathlib.Path) -> float:
        """Return the time of last modification of path. The return value is a 
        floating point number giving the number of seconds since the epoch."""
        return os.path.getmtime(str(path))

# Stores the location and the checksum of a Bricks bootstrap file (cip.py or bootstrap.py).
BootstrapFileInfo = typing.NamedTuple("BootstrapFileInfo", [
       ("href", str),  # Bootstrap file URL
       ("sha256", str) # Bootstrap file SHA256 hash
])

# Stores file information about cip.py and bootstrap.py
BootstrapFiles = typing.NamedTuple("BootstrapFiles", [
    ("cip_py", BootstrapFileInfo),
    ("bootstrap_py", BootstrapFileInfo)
])

# Stores pip config properties
PipConfig = typing.NamedTuple("PipConfig", [
    ("pypi_repo", str)
])

# Stores bootstrap info, in particular the URLs and
# checksums of the latest versions of cip.py and bootstrap.py
BootstrapInfo = typing.NamedTuple("BootstrapInfo", [
    ("bootstrap_files", BootstrapFiles),
    ("pip_config", PipConfig)
])

def calculate_sha256_for_url(url: str, network_access: NetworkAccess) -> str:
    """Calculate the SHA256 checksum for the resource under the given url.
       The resource is considered to be a text file in UTF-8 encoding.
       The function strips out all end-of-line charachters to avoid differences
       between the operating systems.
       The checksum is returned as an upper case hex string.
    """
    binary_content = network_access.read(url)
    text_content = binary_content.decode("utf-8")

    h = hashlib.sha256()
    for line in text_content.splitlines():
        h.update(bytes(line, "utf-8"))

    return h.hexdigest().upper()

class BootstrapFileInfoFactory:
    """Factory for the creation of BootstrapFileInfo instances"""
    def __init__(self, network_access: NetworkAccess):
        self.__network_access = network_access

    def create(self, bootstrap_file_info_dict: typing.Dict[str, typing.Dict]) -> BootstrapFileInfo:
        """Create a BootstrapFileInfo instance.
        :param typing.Dict[str, typing.Dict] bootstrap_file_info_dict:
            Dictionary describing the bootstrap file info, has the following format:
            {
                "href": "...",
                "sha256": "..."
            }
            Note that the "sha256" property is optional.
        :return: BootstrapFileInfo instance
        """
        href = bootstrap_file_info_dict["href"]
        
        if "sha256" in bootstrap_file_info_dict:
            sha256 = bootstrap_file_info_dict["sha256"]
        else:
            sha256 = calculate_sha256_for_url(href, self.__network_access)

        return BootstrapFileInfo(href, sha256)

class BootstrapFilesFactory:
    """Factory for the creation of RepositoryFiles instances"""
    def __init__(self, network_access: NetworkAccess):
        self.__network_access = network_access

    def create(self, bootstrap_files: typing.Dict[str, typing.Dict]) -> BootstrapFiles:
        """Create a BootstrapFiles instance from a dictionary.
        
        :param typing.Dict[str, typing.Dict] bootstrap_files:
            Dictionary describing the bootstrap files, has the following format:
            {
              "bootstrap_py": {
                  <bootstrap.py file info>
              },
              "cip_py": {
                  <cip.py file info>
              }
            }
        :return: BootstrapFiles instance
        """
        bootstrap_file_info_factory = BootstrapFileInfoFactory(self.__network_access)
        cip_py_file_info = bootstrap_file_info_factory.create(bootstrap_files["cip_py"])
        bootstrap_py_file_info = bootstrap_file_info_factory.create(bootstrap_files["bootstrap_py"])
        
        return BootstrapFiles(cip_py_file_info, bootstrap_py_file_info)

class PipConfigFactory:
    """Factory for the creation of PipConfig instances"""
    def create(self, pip_config_dict: typing.Dict[str, str]) -> PipConfig:
        """Create a PipConfig instance from a dictionary.
        
        :param typing.Dict[str, typing.Dict] pip_config_dict:
            Dictionary describing the pip config, has the following format:
            {
                "pypi_repo": <PyPI repo URL>
            }
        :return: PipConfig instance
        """
        pypi_repo = pip_config_dict["pypi_repo"]

        return PipConfig(pypi_repo)

def get_cip_config_dir() -> pathlib.Path:
    return pathlib.Path.home() / "cip_config_dir"

def get_bootstrap_config_dir() -> pathlib.Path:
    return get_cip_config_dir() / "bootstrap"

class BootstrapInfoFactory:
    """Factory for the creation of BootstrapInfo instances"""

    CENTRAL_CONFIGURATION_ENDPOINT = "https://cip-config.cmo.conti.de/v2/configuration/bricks/bootstrap/1.0/configurations/bootstrap.json" # nopep8
    BOOTSTRAP_INFO_PATH = get_bootstrap_config_dir() / "bootstrap.json"

    def __init__(self, network_access: NetworkAccess = None,
                 central_configuration_endpoint: str = None):
        if network_access:
            self.__network_access = network_access
        else:
            self.__network_access = HttpNetworkAccess()

        if central_configuration_endpoint:
            self.__central_configuration_endpoint = central_configuration_endpoint
        else:
            self.__central_configuration_endpoint = self.CENTRAL_CONFIGURATION_ENDPOINT

    def is_local_bootstrap_info_file_up_to_date(self, bootstrap_local_config, 
                                     filesystem_access: FilesystemAccess = None) -> bool:
        return filesystem_access.exists(self.BOOTSTRAP_INFO_PATH)\
               and (time.time() - filesystem_access.getmtime(self.BOOTSTRAP_INFO_PATH)
                    < bootstrap_local_config.max_bootstrap_info_age)

    def create_from_file(self, bootstrap_local_config, filesystem_access: FilesystemAccess) -> BootstrapInfo:
        if not self.is_local_bootstrap_info_file_up_to_date(bootstrap_local_config, filesystem_access):
            download_file(self.__central_configuration_endpoint, self.BOOTSTRAP_INFO_PATH, False,
                          self.__network_access, filesystem_access)

        json_bytes = filesystem_access.read(self.BOOTSTRAP_INFO_PATH)
        json_str = json_bytes.decode("UTF-8")
        bootstrap_info_root_dict = json.loads(json_str)
        bootstrap_info_dict = bootstrap_info_root_dict["bootstrap_info"]
        
        bootstrap_files_factory = BootstrapFilesFactory(self.__network_access)
        bootstrap_files = bootstrap_files_factory.create(bootstrap_info_dict["bootstrap_files"])

        pip_config_factory = PipConfigFactory()
        pip_config = pip_config_factory.create(bootstrap_info_dict["pip_config"])

        return BootstrapInfo(bootstrap_files, pip_config)

def calculate_sha256_for_file(path: pathlib.Path, 
                              filesystem_access: FilesystemAccess = None) -> str:
    """Calculate the SHA256 checksum for the resource under the given path.
       The resource is considered to be a text file in UTF-8 encoding.
       The function strips out all end-of-line charachters to avoid differences
       between the operating systems.
       The checksum is returned as an upper case hex string.
    """
    if not filesystem_access:
        filesystem_access = PhysicalFilesystemAccess()

    binary_content = filesystem_access.read(path)
    text_content = binary_content.decode("utf-8")

    h = hashlib.sha256()
    for line in text_content.splitlines():
        h.update(bytes(line, "utf-8"))

    return h.hexdigest().upper()

def is_file_up_to_date(path: pathlib.Path, 
                       bootstrap_file_info: BootstrapFileInfo, 
                       filesystem_access: FilesystemAccess = None)\
                       -> bool:
    """Check whether the SHA256 hash of the file under "path" is equal to the
    SHA256 hash specified in "bootstrap_file_info"."""
    return calculate_sha256_for_file(path, filesystem_access) == bootstrap_file_info.sha256.upper()

def download_file(url: str, path: pathlib.Path, 
                  dont_fail_on_download_error_if_file_already_exists: bool = False,
                  network_access: NetworkAccess = None, 
                  filesystem_access: FilesystemAccess = None,
                  retries = DEFAULT_DOWNLOAD_RETRIES) -> bool:
    """Download the file from the given url and store it under path.
    Return True if download was successful and False otherwise."""
    if not network_access:
        network_access = HttpNetworkAccess()

    if not filesystem_access:
        filesystem_access = PhysicalFilesystemAccess()

    try:
        binary_content = network_access.read(url)
        filesystem_access.write(path, binary_content)
        return True
    except URLError:
        if dont_fail_on_download_error_if_file_already_exists and filesystem_access.exists(path):
            return False
        else:
            if retries < 1:
                raise
            else:
                seconds_to_sleep = random.randint(MIN_SECONDS_BETWEEN_DOWNLOAD_RETRIES, MAX_SECONDS_BETWEEN_DOWNLOAD_RETRIES)

                if retries > 0:
                    retry_and_attempts_msg = " Retrying in {seconds} seconds ({attempts} attempts remaining)..."\
                        .format(seconds=seconds_to_sleep, attempts=retries)
                else:
                    retry_and_attempts_msg = ""
                
                print("\nUnable to download {href}.{retry_and_attempts_msg}"
                    .format(href=url, retry_and_attempts_msg=retry_and_attempts_msg))
                time.sleep(seconds_to_sleep)
                return download_file(url, path, dont_fail_on_download_error_if_file_already_exists, network_access, filesystem_access, retries - 1)

def update_file_if_needed(path: pathlib.Path,
                          bootstrap_file_info: BootstrapFileInfo,
                          dont_fail_on_download_error_if_file_already_exists: bool,
                          suppress_message_if_update_not_needed: bool,
                          network_access: NetworkAccess = None, 
                          filesystem_access: FilesystemAccess = None,
                          retries = DEFAULT_DOWNLOAD_RETRIES):
    """If the SHA256 hash of the local file under "path" differs from the
       SHA256 specified in "bootstrap_file_info", download the latest version
       of the file from the URL specified in "bootstrap_file_info" and store it
       under "path".
    """
    if not network_access:
        network_access = HttpNetworkAccess()

    if not filesystem_access:
        filesystem_access = PhysicalFilesystemAccess()

    if not filesystem_access.exists(path) \
       or not is_file_up_to_date(path, bootstrap_file_info, filesystem_access):
        try:
            success = download_file(bootstrap_file_info.href, path, dont_fail_on_download_error_if_file_already_exists, network_access, filesystem_access, retries)
            if success:
                print("\nReplaced \"{path}\" with the latest version.\n".format(path=str(path)))
            elif filesystem_access.exists(path):
                print("\nUnable to download {href}. Will try to proceed with the existing {path}".format(href=bootstrap_file_info.href, path=str(path)))
        except URLError:
            if dont_fail_on_download_error_if_file_already_exists and filesystem_access.exists(path):
                print("\nUnable to download {href}. Will try to proceed with the existing {path}".format(href=bootstrap_file_info.href, path=str(path)))
            else:
                raise
    elif not suppress_message_if_update_not_needed:
        print("\n\"{path}\" is already up-to-date.\n    -> No update performed.\n".format(path=str(path)))

class BootstrapLocalConfig:
    VERSION = 1
    DEFAULT_MAX_LOCAL_BOOTSTRAP_INFO_AGE_IN_SECONDS = 6 * 3600
    BOOTSTRAP_LOCAL_CONFIG_PATH = get_bootstrap_config_dir() / "config.json"

    def __init__(self, config_dict):
        """Create an instance of BootstrapLocalConfig from the given dictionary"""
        self._version = int(config_dict["version"])
        self._max_bootstrap_info_age = int(config_dict["max_bootstrap_info_age_in_seconds"])

    @classmethod
    def create(cls, filesystem_access: FilesystemAccess = None):
        """Create an instance of BootstrapLocalConfig by reading the default
        bootstrap local config file"""
        if not filesystem_access:
            filesystem_access = PhysicalFilesystemAccess()

        if not filesystem_access.exists(cls.BOOTSTRAP_LOCAL_CONFIG_PATH):
            cls.write_default_bootstrap_local_config(filesystem_access)

        config_bytes = filesystem_access.read(cls.BOOTSTRAP_LOCAL_CONFIG_PATH)
        config_str = config_bytes.decode("utf-8")
        config_dict = json.loads(config_str)

        if int(config_dict["version"]) != cls.VERSION:
            cls.write_default_bootstrap_local_config(filesystem_access)
            config_bytes = filesystem_access.read(cls.BOOTSTRAP_LOCAL_CONFIG_PATH)
            config_str = config_bytes.decode("utf-8")
            config_dict = json.loads(config_str)

        return BootstrapLocalConfig(config_dict)

    @classmethod
    def write_default_bootstrap_local_config(cls, filesystem_access: FilesystemAccess = None):
        """Create a default boostrap local config file"""
        print("writing default Bricks bootstrap config file: {}".format(cls.BOOTSTRAP_LOCAL_CONFIG_PATH))
        config_dict = {
            "version": cls.VERSION,
            "max_bootstrap_info_age_in_seconds": cls.DEFAULT_MAX_LOCAL_BOOTSTRAP_INFO_AGE_IN_SECONDS
        }
        
        config_str = json.dumps(config_dict, indent=4)
        filesystem_access.write(cls.BOOTSTRAP_LOCAL_CONFIG_PATH, bytes(config_str, "utf-8"))

    @property
    def version(self) -> int:
        """Return file format version"""
        return self._version

    @property
    def max_bootstrap_info_age(self) -> int:
        """Return maximum age of the local bootstrap info (in seconds) before 
        it is considered out-of-date."""
        return self._max_bootstrap_info_age

class BootstrapSession:
    """Provides access to the local config and the central configuration bootstrap info"""
    def __init__(self, filesystem_access: FilesystemAccess = None, network_access: NetworkAccess = None):
        if not filesystem_access:
            filesystem_access = PhysicalFilesystemAccess()
        if not network_access:
            network_access = HttpNetworkAccess()
        
        self.__bootstrap_local_config = BootstrapLocalConfig.create(filesystem_access)
        
        bootstrap_info_factory = BootstrapInfoFactory(network_access)
        self.__bootstrap_info\
            = bootstrap_info_factory.create_from_file(self.__bootstrap_local_config, filesystem_access)

    @property
    def bootstrap_local_config(self) -> BootstrapLocalConfig:
        return self.__bootstrap_local_config

    @property
    def bootstrap_info(self) -> BootstrapInfo:
        return self.__bootstrap_info


###############################################################################
# END: Common code section for cip.py and bootstrap.py
###############################################################################


# the versions here are quite important since we use them
# to bypass the costly check for network
PIP_VERSION = '18.1'

# The version of bricks bootstrap executables to use.
# Don't forget to update the checksums when updating the bootstrap exe version.
BRICKS_BOOTSTRAP_EXE_VERSION = "0.4.5"
# The bricks bootstrap executable checksums are available in the following
# release specific file:
# https://github-am.geo.conti.de/ADCU-CIP/bricks_bootstrap_exe/releases/download/<VERSION>/checksums.txt
BRICKS_BOOTSTRAP_EXE_CHECKSUMS = {
    "nt": "a14d26177fc9d0258c675583f74b98931d8da3be4216ed21ca85db8994180f3b",
    "posix": "869d906ff7c10823b30cc3f06b783665605cf146e585f35b93e1a0f3e8c1053a"
}

__BINARY_SUBDIR = {
    'nt': 'Scripts',
    'posix': 'bin'
}

__REPO_ROOT_PATH = pathlib.Path(__file__).parent.parent.resolve()
__DEFAULT_VIRTUAL_ENV_PATH = __REPO_ROOT_PATH / "venv"
__DEFAULT_CIP_PY_PATH = __REPO_ROOT_PATH / "scripts" / "cip.py"
__USER_CONFIG_DIR_ENV = "CIP_USER_CONFIG_DIR"
__DEFAULT_CIP_WORKAREA_PATH = __REPO_ROOT_PATH / "conan_workarea"


def executable_extension() -> str:
    """return the OS-specific executable extension"""
    return ".exe" if os.name == "nt" else ""


def bricks_bootstrap_exe_url(version: str, extension: str) -> str:
    """return URL of the Bricks bootstrap executable with the specified version
       and executable extension"""
    return "/".join([
        "https://github-am.geo.conti.de",
        "ADCU-CIP",
        "bricks_bootstrap_exe",
        "releases",
        "download",
        "v{version}",
        "bricks{extension}",
    ]).format(version=version, extension=extension)


def bricks_bootstrap_exe_path(
    bootstrap_config_dir: pathlib.Path,
    version: str,
    extension: str,
) -> pathlib.Path:
    """return path to the installation location of the Bricks bootstrap executable"""
    return bootstrap_config_dir / "exe" / \
        version / "bricks{extension}".format(extension=extension)

def get_cip_py_update_message(cip_py_path: pathlib.Path) -> str:
    """Generate message that cip.py is outdated and give update instructions"""
    result = StringIO()
    cip_py_path_normalized = os.path.normpath(str(cip_py_path))
    message = """\nThe cip.py script in the current project is outdated!
Please, update it as soon as possible using the following command:
  {python} {cip_py} --update-cip-py --cip-py={cip_py}

The execution will now continue.\n\n""".format(python=sys.executable, cip_py=cip_py_path_normalized)

    line_lenghts = map(len, message.splitlines())
    longest_line_length = reduce(max, line_lenghts)

    format_top_and_bottom = "!!!!!{:!^%d}!!!!!" % longest_line_length
    top_and_bottom_line = format_top_and_bottom.format(" WARNING ")
    result.write("\n" + top_and_bottom_line + "\n")

    format_string = "!!!  {:<%d}  !!!\n" % longest_line_length
    for line in message.splitlines():
        result.write(format_string.format(line))
    result.write(top_and_bottom_line + "\n")

    return result.getvalue()


def show_cip_py_update_info(cip_py_path: pathlib.Path):
    """Inform the user that cip.py is outdated and give update instructions."""
    print(get_cip_py_update_message(cip_py_path))


def check_cip_py(cip_py_path: pathlib.Path, cip_py_file_info: BootstrapFileInfo):
    """Check whether the invoking cip.py is outdated. If it is outdated and
    display a warning to the user.
    """
    if not cip_py_path.exists() or not is_file_up_to_date(cip_py_path, cip_py_file_info):
        show_cip_py_update_info(cip_py_path)


def parse_cip_py_path(path: str) -> pathlib.Path:
    """Return a pathlib.Path instance representing the passed cip.py path
       in string form."""
    return pathlib.Path(os.path.splitext(path)[0] + ".py")


def parse_known_args(args: typing.List[str]):
    """Parse the command line arguments in "args" and return a triple-component
       tuple whose first component contains an instance of
       argparse.ArgumentParser, the second component being an object with its
       properties set corresponding to the passed command line arguments and
       its third component containing a list of extra command line arguments
       not known by the argument parser."""
    parser = argparse.ArgumentParser(
        description='Bootstrap for CIP Build System 4.x',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        add_help=False
    )

    parser.add_argument(
        '--venv-directory',
        dest='venv_dir',
        type=lambda p: pathlib.Path(p).resolve(),
        default=__DEFAULT_VIRTUAL_ENV_PATH,
        help='Specify the location of virtual environment location'
    )
    parser.add_argument(
        '--bootstrap-py-help',
        default=False,
        action='store_true',
        help='Define help for the bootstrap script itself'
    )
    parser.add_argument(
        '--update-cip-py',
        action="store_true",
        help="Update the running cip.py script"
    )
    parser.add_argument(
        '--cip-py',
        type=parse_cip_py_path,
        default=__DEFAULT_CIP_PY_PATH,
        help="Path to cip.py script to be updated"
    )
    parser.add_argument(
        '--conan-workarea',
        dest='cip_workarea',
        type=lambda p: pathlib.Path(p).resolve(),
        default=__DEFAULT_CIP_WORKAREA_PATH,
        help='Specify the location of conan workarea'
    )

    res_args, passed_args = parser.parse_known_args(args)
    return parser, res_args, passed_args


def calculate_sha256_for_binary_file(path: pathlib.Path,
                                     filesystem_access: FilesystemAccess = None) -> str:
    """Calculate the SHA256 checksum for the resource under the given path.
       The resource is considered to be a text file in UTF-8 encoding.
       The function strips out all end-of-line charachters to avoid differences
       between the operating systems.
       The checksum is returned as an upper case hex string.
    """
    if not filesystem_access:
        filesystem_access = PhysicalFilesystemAccess()

    binary_content = filesystem_access.read(path)

    h = hashlib.sha256()
    h.update(binary_content)

    return h.hexdigest().upper()


def is_binary_file_up_to_date(path: pathlib.Path,
                              bootstrap_file_info: BootstrapFileInfo, 
                              filesystem_access: FilesystemAccess = None)\
                              -> bool:
    """Check whether the SHA256 hash of the binary file under "path" is equal
       to the SHA256 hash specified in "bootstrap_file_info"."""
    return calculate_sha256_for_binary_file(path, filesystem_access) ==\
        bootstrap_file_info.sha256.upper()


def update_binary_file_if_needed(path: pathlib.Path,
                                 bootstrap_file_info: BootstrapFileInfo,
                                 dont_fail_on_download_error_if_file_already_exists: bool,
                                 suppress_message_if_update_not_needed: bool,
                                 network_access: NetworkAccess = None, 
                                 filesystem_access: FilesystemAccess = None,
                                 retries = DEFAULT_DOWNLOAD_RETRIES):
    """If the SHA256 hash of the local binary file under "path" differs from
       the SHA256 specified in "bootstrap_file_info", download the latest
       version of the file from the URL specified in "bootstrap_file_info" and
       store it under "path".
    """
    if not network_access:
        network_access = HttpNetworkAccess()

    if not filesystem_access:
        filesystem_access = PhysicalFilesystemAccess()

    if not filesystem_access.exists(path) \
       or not is_binary_file_up_to_date(path, bootstrap_file_info, filesystem_access):
        try:
            success = download_file(bootstrap_file_info.href, path, dont_fail_on_download_error_if_file_already_exists, network_access, filesystem_access, retries)
            if success:
                print("\nReplaced \"{path}\" with the latest version.\n".format(path=str(path)))
            elif filesystem_access.exists(path):
                print("\nUnable to download {href}. Will try to proceed with the existing {path}".format(href=bootstrap_file_info.href, path=str(path)))
        except URLError:
            if dont_fail_on_download_error_if_file_already_exists and filesystem_access.exists(path):
                print("\nUnable to download {href}. Will try to proceed with the existing {path}".format(href=bootstrap_file_info.href, path=str(path)))
            else:
                raise
    elif not suppress_message_if_update_not_needed:
        print("\n\"{path}\" is already up-to-date.\n    -> No update performed.\n".format(path=str(path)))


def ensure_bricks_bootstrap_exe_is_installed(
    bricks_bootstrap_url: str,
    bricks_bootstrap_path: pathlib.Path,
    bricks_bootstrap_checksum: str,
    network_access: NetworkAccess,
    filesystem_access: FilesystemAccess,
) -> None:
    """Check whether the specified Bricks bootstrap executable version
       is installed and install it if it's not."""
    bootstrap_file_info = BootstrapFileInfo(
        bricks_bootstrap_url, bricks_bootstrap_checksum)
    update_binary_file_if_needed(bricks_bootstrap_path, bootstrap_file_info,
                                 False, False, network_access, filesystem_access)
    os.chmod(str(bricks_bootstrap_path), 0o755)


def execute_bricks_bootstrap_exe(
    bricks_bootstrap_path: pathlib.Path,
    venv_dir: pathlib.Path,
    cip_workarea: pathlib.Path,
    unparsed_args: typing.List[str],
) -> None:
    """Execute the provided Bricks bootstrap executable with the specified arguments."""
    cmd = [
        bricks_bootstrap_path.as_posix(),
        "--venv-directory", venv_dir.as_posix(),
        "--conan-workarea", cip_workarea.as_posix(),
        "--python-exe", pathlib.Path(sys.executable).as_posix(),
    ] + unparsed_args

    # run process without capturing the output
    # as bricks.exe will take care of logging to
    # file; we can enjoy just console output without
    # care about capturing it
    subprocess.run(
        cmd,
        shell=False,
        check=True,
        universal_newlines=True,
    )


def main(args: typing.List[str]):
    """download, install and execute the "cip-build-system" Python package"""
    parser, res_args, passed_args = parse_known_args(args)

    if res_args.bootstrap_py_help:
        parser.print_help()
    elif res_args.update_cip_py:
        bootstrap_session = BootstrapSession()
        bootstrap_info = bootstrap_session.bootstrap_info
        update_file_if_needed(res_args.cip_py,
                              bootstrap_info.bootstrap_files.cip_py, False,
                              False)
    else:
        bootstrap_session = BootstrapSession()
        bootstrap_info = bootstrap_session.bootstrap_info
        check_cip_py(res_args.cip_py, bootstrap_info.bootstrap_files.cip_py)
        bootstrap_exe_path = bricks_bootstrap_exe_path(get_bootstrap_config_dir(),
            BRICKS_BOOTSTRAP_EXE_VERSION, executable_extension())
        ensure_bricks_bootstrap_exe_is_installed(
            bricks_bootstrap_exe_url(
                BRICKS_BOOTSTRAP_EXE_VERSION, executable_extension()),
            bootstrap_exe_path,
            BRICKS_BOOTSTRAP_EXE_CHECKSUMS[os.name],
            HttpNetworkAccess(), PhysicalFilesystemAccess())
        execute_bricks_bootstrap_exe(
            bootstrap_exe_path, res_args.venv_dir,res_args.cip_workarea, passed_args)


if __name__ == '__main__':
    main(sys.argv[1:])
