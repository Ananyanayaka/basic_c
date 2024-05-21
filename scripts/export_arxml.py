#This script parses the passed arxml file and filter it out based on the passed platform.
#It also clears the variation points from arxml in order to be compatible with DaVinci.
from pathlib import Path
from io import FileIO
from xml.dom import minidom
import os
import warnings
import sys 
import argparse

try:
    import yaml
except:
    os.system("pip install yaml")
    import yaml

from yaml.loader import SafeLoader

debug = False

#function to remove VP
def removeVp(vp):
  parent = vp.parentNode
  #each element has a text type sibling node containing new line character
  #which is removed to eliminate empty lines in arxml
  parent.removeChild(vp.previousSibling)
  parent.removeChild(vp)

#function to remove VP parent
def removeVpParent(vp):
  parent1 = vp.parentNode
  parent2 = parent1.parentNode
  #each element has a text type sibling node containing new line character
  #which is removed to eliminate empty lines in arxml
  parent2.removeChild(parent1.previousSibling)
  parent2.removeChild(parent1)

#Function to parse the VariantHandling arxml and extract the platforms and the system constants
def getVariantHandling(inPath):
  print(inPath)
  varHndlArxml = minidom.parse(FileIO(inPath, "rb"))
  varhndl = dict()
  #get platform_CompuMethod element
  if varHndlArxml.getElementsByTagName("COMPU-METHOD"):
    for cm in varHndlArxml.getElementsByTagName("COMPU-METHOD"):
      if cm.getElementsByTagName("SHORT-NAME")[0].firstChild.data == "platform_CompuMethod":
        if cm.getElementsByTagName("VT"):
          for vt in cm.getElementsByTagName("VT"):
            platform = vt.firstChild.data
            sysConst = vt.parentNode.previousSibling.previousSibling.firstChild.data
            if debug: 
              print("platform", platform)
              print("sysconst", sysConst)
            varhndl[platform] = sysConst
          return varhndl
  warnings.warn('Variant Handling missing or invalid!')
  return varhndl

def filterArxml(inPath, outPath, variant):
  
  arxmlName = inPath.name

  #parse the passed arxml
  orgArxml = minidom.parse(FileIO(inPath, "rb"))

  #create a list of VP (variation point) elements
  vPElements = orgArxml.getElementsByTagName("VARIATION-POINT")
  if debug: print(vPElements)

  #iterate through each VP element.
  #If the platform does not match the VP condition formula , remove the  VP parent element.
  #The condition formula in VP looks like following:
  #<SW-SYSCOND BINDING-TIME="SYSTEM-DESIGN-TIME"><SYSC-REF DEST="SW-SYSTEMCONST">/VAR/PLATFORM</SYSC-REF>==2</SW-SYSCOND>
  #The platform is compared with the value after "==".
  for vp in vPElements:

    if debug: 
      parent = vp.parentNode
      print("VP parent node - ", parent)

    if debug: 
      if parent.getElementsByTagName("SHORT-NAME"):
        print(parent.getElementsByTagName("SHORT-NAME"))
        shortName = parent.getElementsByTagName("SHORT-NAME")[0]
        print("VP parent node short name - ", shortName.firstChild.data)

    #get the SW-SYSCOND element from vp element
    if vp.getElementsByTagName("SW-SYSCOND"):
      sysCond = vp.getElementsByTagName("SW-SYSCOND")[0]
      #check that SW-SYSCOND is not empty
      if sysCond.childNodes:

        if debug: 
          print("VP has Sys cond")
          print(sysCond.childNodes)

        #iterate through each condition in SW-SYSCOND
        for node in sysCond.childNodes:
          # check if the VP contains condition formula
          if node.nodeType == node.TEXT_NODE:
            if str(variant) in node.data:
              #Variation point is same as platform.

              if debug: print("Variation point is same as platform.")
              #Remove only the VP and keep the parent

              removeVp(vp)
              toBeRemoved = False
              break
            else:
              if debug: print("Platform does not match the VP condition. Set flag to remove the parent of VP")

              #Platform does not match the VP condition. Set flag to remove the parent of VP
              toBeRemoved = True

        # proceed to remove the VP parent if it does not belong to platform
        if toBeRemoved:

          if debug: print("remove the parent")

          removeVpParent(vp)

    else:
      #SW-SYSCOND is empty. Remove the VP but keep the parent
      removeVp(vp)

  if debug: print("export the new filtered arxml")
  
  #export the new filtered arxml
  with open(outPath + arxmlName, "w") as newArxml:
    newArxml_byte = orgArxml.toxml(encoding = 'UTF-8')
    newArxml_string = newArxml_byte.decode('UTF-8')
    #to split arxml header from namespace
    newArxml_list = newArxml_string.split("?>")
    delimiter = "?>\n"
    newArxml_string = delimiter.join(newArxml_list)
    newArxml.write(newArxml_string)
    print (arxmlName, " successfully exported.")


parser = argparse.ArgumentParser()
parser.add_argument("--arxml_dir_path",     dest="arxml_dir_path",      type=str, required=True)
parser.add_argument("--export_dir_path",    dest="export_dir_path",     type=str, required=True)

arguments = parser.parse_args()

variantHandlingArxml = arguments.arxml_dir_path + 'VariantHandling.arxml'

ab_vars = getVariantHandling(variantHandlingArxml)
if debug: print(ab_vars)

# export arxmls for each platform available in VariantHandling.arxml
for platform, abSysConst in ab_vars.items():
  exportPath = arguments.export_dir_path + platform +'/'
  if debug: print(exportPath)

  #create export directory if it does not exist
  if  not  os.path.exists(exportPath):
    os.makedirs(exportPath)

  #create a list of all available arxmls
  arxmls = Path(arguments.arxml_dir_path).glob('*.arxml')

  #Filter and export each arxml
  for arxml in arxmls:
    if debug:print(arxml)
    filterArxml(arxml, exportPath, abSysConst)

