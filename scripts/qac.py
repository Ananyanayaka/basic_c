#!/usr/bin/env python 
import os

try:
    import yaml
except:
    try:
        os.system("pip install yaml")
        import yaml
    except:
        os.system("pip install pyyaml")
        import yaml
 
from yaml.loader import SafeLoader

dir_path = os.path.abspath(os.path.join(os.path.dirname(__file__),".."))
#print("dir_path: " + dir_path)

if False == os.path.isfile("conf/qaf.yml"):
    print("ERROR: QAF is not available in this repository.")
    input("Press any key to exit...")
    exit()

#todo search for latest version of static code analysis launcher
static_code_analysis_launcher_path = "C:/cip_tools/static_code_analysis_launcher/1.2.2/scripts/qac_launcher.ps1"
if False == os.path.isfile(static_code_analysis_launcher_path):
    print("ERROR: Static code analysis is not installed.")
    input("Press any key to exit...")
    exit()


# Open and load the build.yml file
with open('conf/build.yml') as f:
    data = yaml.load(f, Loader=SafeLoader)
    #print(data)
    
    #read the name of the repo
    repo_name_string = data['name']
    #print("repo_name_string: " + repo_name_string)
    
    #read the major version
    major_version_string = str(data['major_version'])
    #print("major_version_string: " + major_version_string)
    
    #Select a variant to build
    variants = data['variants'] #variants is of type dict
    print("Available variants: ")
    i = 0
    for variant in variants:
      i+=1
      print(str(i) + ": " + variant)
        
    use_variant = int(input("Select variant: "))
    selected_variant = list(variants)[use_variant-1]#cast the dict to a list, to access the elements
    print()
    
    #Select a platform to build
    try:
      #old build.yml (do not have the production key)
      platforms = variants[selected_variant]['build_platforms']#platforms is a list now
      platforms_length = len(platforms)
      if platforms_length > 1:
          i = 0
          for platform in platforms:
            build = platforms[i]#build is a string
            i+=1
            print(str(i) + ": " + build)
          use_build = int(input("Select build: "))
          platform_string = platforms[use_build-1]#platform_string is a string
      else:
          platform_string = platforms[0]
          print("auto select: " + platform_string)
      print()
      
    except:
      #new build.yml
      platforms = variants[selected_variant]['production']['platforms']#platforms is a list now
      platforms_length = len(platforms)
      if platforms_length > 1:
          i = 0
          for platform in platforms:
            build = platforms[i]['build']#build is a string
            i+=1
            print(str(i) + ": " + build)
        
          use_build = int(input("Select build: "))
          platform_string = platforms[use_build-1]['build']#platform_string is a string
      else:
          platform_string = platforms[0]['build']
          print("auto select: " + platform_string)
      print()


    #Ready to build the build cmd  
    json_file_path = "conan_workarea/build." + repo_name_string +"." + selected_variant + "." + major_version_string + ".0.0-fallback." + platform_string + "/cip_build/compile_commands.json"
    qac_string = static_code_analysis_launcher_path + " conf/qaf.yml --gui --json conan_workarea/build." + repo_name_string +"." + selected_variant + "." + major_version_string + ".0.0-fallback." + platform_string + "/cip_build/compile_commands.json --bricks_variant " + selected_variant + " --bricks_build_platform " + platform_string
    
    print()
    
    #loop control variables
    done = False
    first_time_build = True
    
    while done == False:
    
        #Check if build is available.
        if True == os.path.isfile(json_file_path):
            print("Calling: " + qac_string)
            os.system("start /D " + dir_path  + " powershell " + qac_string)
            done = True
        else:
            #Offer build only once.
            if True == first_time_build:
                build_decision = str(input("Project is not available. Do you want to build it now? (y/n):  "))
                if build_decision == "y":
                    build_string = "py scripts/cip.py build --use-cached-graph --variant " + selected_variant + " --platform " + platform_string
                    print("Calling: " + build_string)
                    os.system(build_string)
                    first_time_build = False
                else:
                    done = True
                    
            else:
                    done = True
                    print()
                    print("ERROR: File compile_commands.json still was not found. Assure QAC is configured correctly")
    
#@uic12551