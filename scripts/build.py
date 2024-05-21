#!/usr/local/bin/python3
# encoding: utf-8
'''
bricks_command_build -- Tool for creating a bricks command based on the build.yml file

@license:    Proprietary
@copyright:  2022 Continental. All rights reserved.
@author: Darko Kojic
@contact: darko.kojic@continental-corporation.com
'''
# check imports
try:
  import argparse
  import os
  import sys
  import yaml
except ImportError as e:
  print("Missing package: " + e.name)
  print("The easiest way to install missing dependencies is using pip install:")
  print("pip install -r requirements.txt")
  exit(1)

all_items = "all items"
done_string = "selection finished"


#########################################################
#
# Classes
#
#########################################################
class BricksBuildYml:
  '''
  Bricks build.yml parsing class
  '''
  __conf = None
  __conf_path = None
  __schema_class = None
  __schema_version = 1

  def __init__(self, path):
    '''
    Constructor takes build.yml path and loads the configuration

    @param path: build.yml path
    @raise Exception: Error parsing file
    '''
    try:
      with open(path, 'r') as stream:
        self.__conf = yaml.safe_load(stream)
        self.__conf_path = path
    except yaml.YAMLError as ex:
      raise Exception("Error parsing " + path + "\n" + str(ex))

    # get schema version
    try:
      self.__schema_version = self.__conf["schema_version"]
    except:
      pass

    # load correct scheme class
    if self.__schema_version == 1:
      self.__schema_class = self.BricksBuildYml_Schema_1(self)
    elif self.__schema_version == 2:
      self.__schema_class = self.BricksBuildYml_Schema_2(self)
    else:
      raise Exception("Unknown schema")

  def get_conf(self):
    '''
    Get build.yml configuration

    @return: configuration
    @rtype: dictionary
    '''
    return self.__conf

  def get_schema_version(self):
    '''
    Get schema version from build.yml

    @return: Schema version
    @rtype: integer
    '''
    return self.__schema_version

  def get_name(self):
    '''
    Get package name from build.yml

    @return: package name
    @rtype: string
    '''
    return self.__conf["name"]

  def get_major_version(self):
    '''
    Get package major version from build.yml

    @return: package major version
    @rtype: string
    '''
    return self.__conf["major_version"]

  def get_variants(self):
    '''
    Get list of variants

    @return: variants
    @rtype: list
    '''
    return self.__schema_class.get_variants()

  def get_build_types(self, variant):
    '''
    Get list of build types for a given variant

    @param variant: name of the variant
    @return: build types
    @rtype: list
    '''
    return self.__schema_class.get_build_types(variant)

  def get_test_labels(self, variant):
    '''
    Get list of test labels for a given variant

    @param variant: name of the variant
    @return: labels
    @rtype: list
    '''
    return self.__schema_class.get_test_labels(variant)

  def get_platforms(self, variant, build_type = None, label = None):
    '''
    Get list of platforms for a given variant

    @param variant: name of the variant
    @param build_type: name of the build type
    @param label: name of the label
    @return: platforms
    @rtype: list
    '''
    if self.__schema_version == 1:
      return self.__schema_class.get_platforms(variant)
    else:
      return self.__schema_class.get_platforms(variant, build_type, label)

  def get_tools(self):
    '''
    Get list of tools for the QAC build

    @return: tool tuples containing variant name and platform name
    @rtype: list of tuples
    '''
    return self.__schema_class.get_tools()

  # internal class for schema 1
  class BricksBuildYml_Schema_1():
    '''
    Bricks build.yml parsing class for schema 1
    '''
    __parent = None
    __conf = None

    def __init__(self, parent):
      '''
      Constructor takes parent class as parameter for taking the configuration

      @param parent: BricksBuildYml
      '''
      self.__parent = parent
      self.__conf = parent.get_conf()

    def get_variants(self):
      '''
      Get list of variants

      @return: variants
      @rtype: list
      '''
      items = []
      for elem in self.__conf["variants"]:
        items.append(elem)
      if len(items) > 1:
        items.insert(0, all_items)
      return items

    def get_platforms(self, variant):
      '''
      Get list of platforms for a given variant

      @param variant: name of the variant
      @return: platforms
      @rtype: list
      '''
      items = []
      elements = (((self.__conf["variants"])[variant])["build_platforms"])
      for elem in elements:
        items.append(elem)
      if len(items) > 1:
        items.insert(0, all_items)
      return items

    def get_test_labels(self, variant):
      '''
      Get list of test labels for a given variant

      @param variant: name of the variant
      @return: labels
      @rtype: list
      '''
      items = []
      for elem in ((((self.__conf["variants"])[variant])["testing"])["types"]):
        items.append(elem)
      if len(items) > 1:
        items.insert(0, all_items)
      return items

    def get_build_types(self, variant):
      '''
      Get list of build types labels for a given variant
      Note: In the build.yml schema 1 there is no documentation type. Documentation can be built on any platform
            targeting PC platform (Windows or Linux). For simplicity, documentation added for every build, but bricks
            will not be able to build it.

      @param variant: name of the variant
      @return: build types
      @rtype: list
      '''
      items = ["all"]
      items += ["production"]
      try:
        dummy = ((((self.__conf["variants"])[variant])["testing"])["types"])
        items += ["testing"]
      except:
        pass
      # only PC (Windows and Linux) builds can be used for documentation, but I cannot remember how to filter them
      # leaving it like this for now
      items += ["documentation"]
      return items

    def get_tools(self):
      '''
      Get list of tools for the QAC build

      @return: tool tuples containing variant name and platform name
      @rtype: list of tuples
      '''
      items = []
      for elem in (self.__conf["tools"])["specs"]:
        try:
          items.append(((elem["match_attributes"])["/variant/name"], (elem["match_attributes"])["/build_platform/name"]))
        except:
          pass
      return items

  # internal class for schema 2
  class BricksBuildYml_Schema_2():
    '''
    Bricks build.yml parsing class for schema 2
    '''
    __parent = None
    __conf = None

    def __init__(self, parent):
      '''
      Constructor takes parent class as parameter for taking the configuration

      @param parent: BricksBuildYml
      '''
      self.__parent = parent
      self.__conf = parent.get_conf()

    def get_variants(self):
      '''
      Get list of variants

      @return: variants
      @rtype: list
      '''
      items = []
      for elem in self.__conf["variants"]:
        items.append(elem)
      if len(items) > 1:
        items.insert(0, all_items)
      return items

    def get_build_types(self, variant):
      '''
      Get list of build types labels for a given variant

      @param variant: name of the variant
      @return: build types
      @rtype: list
      '''
      items = []
      for elem in ((self.__conf["variants"])[variant]).keys():
        items.append(elem)
      if len(items) > 1:
        items.insert(0, all_items)
      return items

    def get_test_labels(self, variant):
      '''
      Get list of test labels for a given variant

      @param variant: name of the variant
      @return: labels
      @rtype: list
      '''
      items = []
      for elem in (((self.__conf["variants"])[variant])["testing"]):
        items.append(elem)
      if len(items) > 1:
        items.insert(0, all_items)
      return items

    def get_platforms(self, variant, build_type, label):
      '''
      Get list of platforms for a given variant

      @param variant: name of the variant
      @param build_type: name of the build type
      @param label: name of the label
      @return: platforms
      @rtype: list
      '''
      items = []
      if not label is None:
        elements = (((((self.__conf["variants"])[variant])[build_type])[label])["platforms"])
      else:
        elements = ((((self.__conf["variants"])[variant])[build_type])["platforms"])
      for elem in elements:
        items.append(elem["build"])
      if len(items) > 1:
        items.insert(0, all_items)
      return items

    def get_tools(self):
      '''
      Get list of tools for the QAC build

      @return: tool tuples containing variant name and platform name
      @rtype: list of tuples
      '''
      items = []
      for elem in (self.__conf["tools"])["specs"]:
        try:
          items.append(((elem["match_attributes"])["/variant/name"], (elem["match_attributes"])["/build_platform/name"]))
        except:
          pass
      return items

#########################################################
#
# Program input
#
#########################################################


def get_args():
  descr = """

  Tool for creating bricks build command based on the existing conf/build.yml configuration.
    """

  parser = argparse.ArgumentParser(prog = "bricks_command_build",
                                   formatter_class = argparse.RawTextHelpFormatter,
                                   description = descr,
                                   usage = argparse.SUPPRESS,
                                   add_help = False
                                   )
  parser.add_argument("-b", "--build_yml_path",
                      action = "store",
                      type = str,
                      default = "./conf/build.yml",
                      help = "Path of the build.yml")
  parser.add_argument("-f", "--command_file",
                      action = "store",
                      type = str,
                      default = "./last_build_command",
                      help = "Path of the file where the command is stored")
  parser.add_argument("-e", "--extra_options",
                      action = "store",
                      type = str,
                      help = "Additional bricks build options (must start with the space character")
  parser.add_argument("-q", "--qac_launcher_path",
                      action = "store",
                      type = str,
                      default = get_default_qac_launcher_path(),
                      help = "Path to the directory where qac launcher start script is stored")
  parser.add_argument("-c", "--configure",
                      action = "store_true",
                      default = False,
                      help = "Go to configuration, do not ask for using existing configuration")
  parser.add_argument("-s", "--shell",
                      action = "store",
                      type = str,
                      default = "sh",
                      help = "Name of the shell to be used",
                      choices = ["sh", "ps"])
  parser.add_argument("-p", "--build_config_paths",
                      action = "store",
                      type = str,
                      default = "./scripts/bricks_build_config_list",
                      help = "Path of the file where the list of directory paths used for generating list for --build-config option is stored")

  try:
    args = parser.parse_args()
  except SystemExit:
    print("=========================================")
    parser.print_help()
    sys.exit(1)

  if not os.path.isfile(args.build_yml_path):
    print("Input file " + args.build_yml_path + " does not exists")
    sys.exit(1)

  return args

#########################################################
#
# Utility Functions
#
#########################################################


def get_default_qac_launcher_path():
  '''
  Get correct QAC launcher path. For now, only default Windows and Linux locations are supported. If your
  installation is not in the default path, change the following path location directly on your workstation

  @return: static_code_analysis_launcher path
  @rtype: string
  '''
  qac_launcher_path = ""
  if sys.platform == "linux":
    qac_launcher_path = "~/cip_tools/static_code_analysis_launcher/1.2.0/"
  else:
    qac_launcher_path = "c:/cip_tools/static_code_analysis_launcher/1.2.0/"
  return qac_launcher_path


def get_choice(choice_list):
  '''
  Print a list of items and ask user to choose one of them

  @param choice_list: list of items
  @return: index of selected item
  @rtype: integer
  @raise Exception: Empty list provided
  '''
  if os.name == "posix":
    os.system("clear")
  else:
    os.system("cls")

  if len(choice_list) == 0:
    raise Exception("Empty list provided")

  for i in range(0, len(choice_list)):
    print(str(i + 1) + " - " + str(choice_list[i]))
    i = i + 1
  selected_element = ""

  while not selected_element.isnumeric() or int(selected_element) < 1 or int(selected_element) > len(choice_list):
    selected_element = input("Please type the number of the element you want to select or ENTER for the default value (1):\n")
    if selected_element == "":
      selected_element = "1"

  return int(selected_element)


def get_choice_item(items):
  '''
  Select an element from the list.
  If the list is empty it returns an empty string.
  If the list contains only one element it is automatically selected.
  If the list contains two or more elements the user is presented with a choice.

  @param items: list of elements
  @return: selected element
  @rtype: string
  '''
  if len(items) == 0:
    ret_val = ""
  elif len(items) == 1:
    ret_val = items[0]
  else:
    selected_element = get_choice(items)
    ret_val = items[selected_element - 1]

  return ret_val


def get_cip_launcher():
  '''
  String for starting Bricks buils

  @return: Bricks start command
  @rtype: string
  '''
  cip_script = ""
  if sys.platform == "linux":
    cip_script = "python3"
  else:
    cip_script = "py"
  cip_script += " scripts/cip.py build "
  return cip_script


def get_qac_launcher(launcher_path, shell):
  '''
  Print a list of items and ask user to choose one of them

  @param launcher_path: path of the QAC launcher folder
  @param shell: which shell script is used
  @return: full path of the qac launcher script
  @rtype: string
  @raise Exception: Unknown QAC script extension
  '''

  # remove trailing slash/backslash if present
  launcher_path = launcher_path.rstrip("\\")
  launcher_path = launcher_path.rstrip("/")
  qac_script = launcher_path + "/scripts/qac_launcher."
  if shell == "sh":
    qac_script += "sh"
  elif shell == "ps":
    qac_script += "ps1"
  else:
    raise Exception ("Unknown QAC script extension")

  return qac_script


def write_command(command_file, command):
  '''
  Write command to the file

  @param command_file: path to the file where the command is written
  @param command: command to be written into file
  '''

  f = open(command_file, "w")
  f.write(command)
  f.close()


def write_bricks_command(command_file, command):
  '''
  Write bricks command to the file

  @param command_file: path to the file where the command is written
  @param command: command to be written into file
  '''
  command = command.lstrip(" ")
  write_command(command_file, get_cip_launcher() + " " + command)


def write_qac_command(command_file, command, launcher_path):
  '''
  Write QAC command to the file

  @param command_file: path to the file where the command is written
  @param command: command to be written into file
  @param launcher_path: path of the QAC launcher folder
  '''
  command = command.lstrip(" ")
  write_command(command_file, get_qac_launcher(launcher_path) + " " + command)

#########################################################
#
# Build Functions
#
#########################################################


def build_cmd(file, extra_options = None, variant = None, build_type = None, platform = None, label = None, use_ctc = None):
  '''
  Build and write command based on input

  @param file: path to the file where the command is written
  @param extra_options: extra options to be added to the bricks command
  @param variant: variant from the build.yml to be built
  @param build_type: build type from the build.yml to be built
  @param platform: platform from the build.yml to be built
  @param label: label from the build.yml to be built
  @param use_ctc: does ctc build is needed
  '''
  command = ""
  if not variant is None:
    command += " --variant " + variant
  if not build_type is None:
    command += " --build-type " + build_type
  if not platform is None:
    command += " --platform " + platform
  if not label is None:
    command += " --test-type " + label
  if use_ctc:
    command += " --ctc "
  if extra_options:
    command += " " + extra_options

  write_bricks_command(file, command)


def build_schema_version_1(conf, file, extra_options):
  '''
  Build bricks command for a configuration which is using schema 1

  @param conf: configuration class (BricksBuildYml_Schema_1)
  @param file: path to the file where the command is written
  @param extra_options: extra options to be added to the bricks command
  @return: 0 for success
  @rtype: integer
  '''
  variant = get_choice_item(conf.get_variants())

  if variant == all_items:
    build_cmd(file, extra_options)
  else:
    build_type = get_choice_item(conf.get_build_types(variant))

    if build_type == "production":
      platform = get_choice_item(conf.get_platforms(variant, build_type))
      if platform == all_items:
        build_cmd(file, extra_options, variant)
      else:
        build_cmd(file, extra_options, variant, None, platform)

    elif build_type == "testing":
      platform = get_choice_item(conf.get_platforms(variant))
      label = get_choice_item(conf.get_test_labels(variant))

      if platform == all_items:
        if label == all_items:
          build_cmd(file, extra_options, variant, build_type)
        else:
          build_cmd(file, extra_options, variant, build_type, None, label)
      else:
        if label == all_items:
          build_cmd(file, extra_options, variant, build_type, platform)
        else:
          build_cmd(file, extra_options, variant, build_type, platform, label)

    elif build_type == "documentation":
      platform = get_choice_item(conf.get_platforms(variant))

      if platform == all_items:
        build_cmd(file, extra_options, variant, build_type)
      else:
        build_cmd(file, extra_options, variant, build_type, platform)

    elif build_type == "all":
      build_cmd(file, extra_options, variant)
  return 0


def build_schema_version_2(conf, file, extra_options):
  '''
  Build bricks command for a configuration which is using schema 2

  @param conf: configuration class (BricksBuildYml_Schema_2)
  @param file: path to the file where the command is written
  @param extra_options: extra options to be added to the bricks command
  @return: 0 for success
  @rtype: integer
  '''
  variant = get_choice_item(conf.get_variants())

  if variant == all_items:
    build_cmd(file, extra_options)
  else:
      build_type = get_choice_item(conf.get_build_types(variant))

      if build_type == all_items:
        build_cmd(file, extra_options, variant)
      else:
        use_ctc = False
        label = None

        if build_type == "testing":
          use_ctc = True if (get_choice(["with ctc", "without ctc"]) == 1) else False
          label = get_choice_item(conf.get_test_labels(variant))

        if label == all_items:
          build_cmd(file, extra_options, variant, build_type, None, None, use_ctc)
        else:
          platform = get_choice_item(conf.get_platforms(variant, build_type, label))

          if platform == all_items:
            build_cmd(file, extra_options, variant, build_type, None, label, use_ctc)
          else:
            build_cmd(file, extra_options, variant, build_type, platform, label, use_ctc)
  return 0


def bricks_build(conf, file, extra_options):
  '''
  Build bricks command

  @param conf: build.yml parser object
  @param file: path to the file where the command is written
  @param extra_options: extra options to be added to the bricks command
  @return: 0 for success
  @rtype: integer
  @raise Exception: Schema not supported
  '''
  schema_version = conf.get_schema_version()

  if schema_version == 1:
    return build_schema_version_1(conf, file, extra_options)
  elif schema_version == 2:
    return build_schema_version_2(conf, file, extra_options)
  else:
    raise Exception("Schema not supported")


def append_build_config(file, build_config_paths):
  '''
  Provide a choice for adding additional build-config files to the command
  Multiple select choice is offered

  @param file: path to the file where the command is written
  @param build_config_paths: list of directories for searching for conf/build.yml files
  @return: 0 for success
  @rtype: integer
  '''
  # 3 list are used:
  # - list of paths to build.yml files
  bricks_package_list = []
  # - list for displaying files in the choice
  bricks_package_list_display = []
  # - list of selected packages
  selected_packages = []

  for elem in build_config_paths:
    if os.path.isdir(elem):
      # list all elements
      for sub_dir in os.listdir(elem):
        sub_dir_path = os.path.join(elem, sub_dir)
        # check directories only
        if os.path.isdir(sub_dir_path):
          build_yml = os.path.join(sub_dir_path, "conf/build.yml").replace("\\", "/")
          if os.path.isfile(build_yml):
            # all paths with valid build.yml paths
            bricks_package_list.append(build_yml)
            bricks_package_list_display.append(sub_dir + " (" + elem + ")")
    else:
      print("WARNING: Ignoring invalid path: " + elem)

  # insert an element used to end selection
  bricks_package_list.insert(0, done_string)
  bricks_package_list_display.insert(0, done_string)

  done = False
  while not done:
    selected_item_id = get_choice(bricks_package_list_display)
    selected_item = bricks_package_list[selected_item_id - 1]
    if selected_item != done_string:
      selected_packages.append(selected_item)
      bricks_package_list.pop(selected_item_id - 1)
      bricks_package_list_display.pop(selected_item_id - 1)
    else:
      done = True

  # write command
  if(len(selected_packages) > 0):
    with open(file, "a+") as f:
      for elem in selected_packages:
        f.write(" --build-config " + elem)

  return 0


def qac_build(conf, file, qac_launcher_path, shell):
  '''
  Build QAC command

  @param conf: build.yml parser object
  @param file: path to the file where the command is written
  @param qac_launcher_path:  path of the QAC launcher folder
  @param shell: which shell script is used
  @return: 0 for success / 1 if QAC is not configured for the project
  @rtype: integer
  '''
  tools = conf.get_tools()
  if len(tools) == 0:
    print("QAC is not configured")
    return 1

  selected_project = get_choice(tools)
  project = tools[selected_project - 1]

  command = (get_qac_launcher(qac_launcher_path, shell) +
            " conf/qaf.yml --gui --json " +
            "./conan_workarea/build." + conf.get_name() + "." +
            project[0] + "." +
            str(conf.get_major_version()) + ".0.0-fallback." +
            project[1] +
            "/cip_build/compile_commands.json" +
            " --bricks_variant " + project[0] +
            " --bricks_build_platform " + project[1])

  write_command(file, command)

  return 0


def get_build_config_paths_list(build_config_paths):
  '''
  check if there is a list of directories for --build-config and create a new one if it does not exists

  @param build_config_paths: path to the file with a list of directories to be searched for --build-config elements
  @return: list of paths as strings or empty list
  @rtype: list
  '''
  build_config_paths_list = []
  if os.path.isfile(build_config_paths):
    # read existing file
    with open(build_config_paths) as f:
      lines = f.readlines()
      for line in lines:
          build_config_paths_list.append(line.replace("\\", "/").replace("\n", ""))
  else:
    try:
      f = open(build_config_paths, "w")
      # create a new config file
      print("Please type in paths to be used for searching for build-config (e.g. d:/git/)")
      print("   (only add folders where repositories are cloned, not the full path to build.yml files)")
      print("For finishing input press ENTER")

      selected_element = None
      while selected_element != "":
          # get entry and transform all backslashes to forward slashes
        selected_element = input("\n").replace("\\", "/")
        if selected_element != "":
          build_config_paths_list.append(selected_element)
          f.write(selected_element + "\n")
      f.close()
    except:
      print("Cannot create: " + build_config_paths)

  return build_config_paths_list


def main():
  '''
  Main function

  @return: 0 for success
  @rtype: integer
  '''
  args = get_args()
  print("args: " + args.build_config_paths)
  return_value = 0
  choice_list = []

  build_config_paths_list = get_build_config_paths_list(args.build_config_paths)

  # check if there is a command from previous build available and if we can use it
  if not args.configure:
    if os.path.isfile(args.command_file):
      choice_list.append("use previous build command")

  # bricks build is always active
  choice_list.append("bricks build")

  conf = BricksBuildYml(args.build_yml_path)
  tools = conf.get_tools()
  if len(tools) > 0:
    choice_list.append("qac build")

  build_type = get_choice_item(choice_list)

  if build_type == "bricks build":
    return_value = bricks_build(conf, args.command_file, args.extra_options)
    if return_value == 0 and not len(build_config_paths_list) == 0:
      return_value = append_build_config(args.command_file, build_config_paths_list)
  elif build_type == "qac build":
    return_value = qac_build(conf, args.command_file, args.qac_launcher_path, args.shell)
  else:
    print("Using previous command")

  f = open(args.command_file, "r")
  print(f.readline())
  f.close()
  return return_value


if __name__ == '__main__':
  exit(main())
