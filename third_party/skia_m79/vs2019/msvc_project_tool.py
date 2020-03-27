import os
from argparse import ArgumentParser
import xml.dom.minidom

def change_vcxproj_xml(src_xml_fname, dest_xml_fname): 
  is_custom = False
  is_clcompile = False
  is_clinclude = False
  is_resourcecompile = False
  custom_itemgroup = None
  clcompile_itemgroup = None
  clinclude_itemgroup = None
  resourcecompile_itemgroup = None
  valid_cl_ext_names = ['cc', 'h', 'c', 'cpp', 'inl']
  link_nodes = []
  def_value = ''
  cpp_names = []
  cc_names = []
  doc = xml.dom.minidom.parse(src_xml_fname)
  print("doc = %s" %doc.nodeName)
  print("\tnode = %s" %doc.firstChild.tagName)
  if doc.firstChild.nodeName == 'Project':
    project_node = doc.firstChild
    project_child_nodes = project_node.childNodes    
    for project_child_node in project_child_nodes:
      if project_child_node.nodeType == project_child_node.ELEMENT_NODE:
        print("\t\tproject node = %s" %project_child_node.nodeName)
        if project_child_node.nodeName == 'ItemGroup':          
          itemgroup_node = project_child_node
          itemgroup_child_nodes = itemgroup_node.childNodes
          if itemgroup_node.getAttribute('Label') == 'ProjectConfigurations':
            for itemgroup_child_node in itemgroup_child_nodes:
              if itemgroup_child_node.nodeType == itemgroup_child_node.ELEMENT_NODE:
                if itemgroup_child_node.nodeName == 'ProjectConfiguration':
                  projectconfiguration_node = itemgroup_child_node
                  include_value = projectconfiguration_node.getAttribute('Include')
                  include_value_platform = include_value.rsplit('|', 1)[1]
                  include_value = 'Debug|' + include_value_platform
                  for projectconfiguration_child_node in projectconfiguration_node.childNodes:
                    if projectconfiguration_child_node.nodeType == projectconfiguration_child_node.ELEMENT_NODE:
                      if projectconfiguration_child_node.nodeName == 'Configuration':
                        configuration_node = projectconfiguration_child_node
                        for configuration_child_node in configuration_node.childNodes:
                          configuration_node.removeChild(configuration_child_node)
                        configuration_node.appendChild(doc.createTextNode('Debug'))
                        projectconfiguration_node.setAttribute('Include', include_value) 
            continue
          clcompile_include_values = []
          clinclude_include_values = []
          for itemgroup_child_node in itemgroup_child_nodes:
            if itemgroup_child_node.nodeType == itemgroup_child_node.ELEMENT_NODE:
              print("\t\t\t itemgroup node = %s" %itemgroup_child_node.nodeName)
              if itemgroup_child_node.nodeName == 'CustomBuild':
                if not is_custom:
                  custom_itemgroup = itemgroup_node
                  is_custom = True
                if not is_clcompile:
                  clcompile_itemgroup = doc.createElement('ItemGroup')
                  is_clcompile = True
                include_value = itemgroup_child_node.getAttribute('Include')
                include_value_ext = include_value.rsplit('.', 1)[1]
                if include_value_ext not in valid_cl_ext_names:
                  print("\t\t\t\tinclude value = %s" %include_value)                                      
                if include_value not in clcompile_include_values:
                  clcompile_include_values.append(include_value)
                  clcompile_itemgroup.appendChild(doc.createTextNode('\n    '))                
                  #print("\t\t\t\tinclude value = %s" %include_value)
                  cl_elem = doc.createElement('ClCompile')
                  cl_elem.setAttribute('Include', include_value)
                  if include_value_ext == 'cpp':
                    cpp_name = include_value.rsplit('/', 1)[1]
                    if cpp_name.lower() not in cpp_names:
                        cpp_names.append(cpp_name.lower())
                    else:
                      objectfilename_elem = doc.createElement('ObjectFileName')
                      cpp_dir_name = include_value.rsplit('/', 2)[1]
                      cpp_name_noext = cpp_name.rsplit('.', 1)[0]
                      obj_fname = '$(IntDir)' + cpp_dir_name + '_' + cpp_name_noext + '.obj'
                      objectfilename_elem.appendChild(doc.createTextNode(obj_fname))
                      cl_elem.appendChild(doc.createTextNode('\n      '))
                      cl_elem.appendChild(objectfilename_elem)
                      cl_elem.appendChild(doc.createTextNode('\n    '))
                  clcompile_itemgroup.appendChild(cl_elem)
              elif itemgroup_child_node.nodeName == 'None':
                if not is_custom:
                  custom_itemgroup = itemgroup_node
                  is_custom = True
                if not is_clinclude:
                  clinclude_itemgroup = doc.createElement('ItemGroup')
                  is_clinclude = True
                include_value = itemgroup_child_node.getAttribute('Include')
                include_value_ext = include_value.rsplit('.', 1)[1]
                if include_value_ext not in valid_cl_ext_names:                   
                  print("\t\t\t\tinclude value = %s" %include_value)
                  if include_value_ext == 'def':
                    def_value = include_value
                  if include_value_ext == 'rc':
                    if not is_resourcecompile:
                      resourcecompile_itemgroup = doc.createElement('ItemGroup')
                      is_resourcecompile = True
                    resourcecompile_itemgroup.appendChild(doc.createTextNode('\n    '))
                    rc_elem = doc.createElement('ResourceCompile') 
                    rc_elem.setAttribute('Include', include_value)
                    resourcecompile_itemgroup.appendChild(rc_elem) 
                    continue
                if include_value not in clinclude_include_values:
                  clinclude_include_values.append(include_value)
                  clinclude_itemgroup.appendChild(doc.createTextNode('\n    '))
                  #print("\t\t\t\tinclude value = %s" %include_value)
                  cl_elem = doc.createElement('ClInclude')
                  cl_elem.setAttribute('Include', include_value)
                  clinclude_itemgroup.appendChild(cl_elem)               
        if project_child_node.nodeName == 'Target': 
          project_node.removeChild(project_child_node)
        if project_child_node.nodeName == 'PropertyGroup':
          has_outdir = False
          propertygroup_node = project_child_node
          ipropertygroup_child_nodes = propertygroup_node.childNodes
          for ipropertygroup_child_node in ipropertygroup_child_nodes:
            if ipropertygroup_child_node.nodeType == ipropertygroup_child_node.ELEMENT_NODE:
              print("\t\t\t ipropertygroup node = %s" %ipropertygroup_child_node.nodeName)
              if ipropertygroup_child_node.nodeName == 'OutDir':
                outdir_node = ipropertygroup_child_node
                for outdir_child_node in outdir_node.childNodes:
                  outdir_node.removeChild(outdir_child_node)
                outdir_node.appendChild(doc.createTextNode('$(SolutionDir)$(Platform)\\$(Configuration)\\'))
                has_outdir = True
                break
          if has_outdir:
            cl_elem = doc.createElement('IntDir')
            cl_elem.appendChild(doc.createTextNode('$(Platform)_$(Configuration)\\$(ProjectName)\\'))
            propertygroup_node.appendChild(doc.createTextNode('  ')) 
            propertygroup_node.appendChild(cl_elem)  
            propertygroup_node.appendChild(doc.createTextNode('\n  '))            
        if project_child_node.nodeName == 'ItemDefinitionGroup':
          itemdefinitiongroup_node = project_child_node
          for itemdefinitiongroup_child_node in itemdefinitiongroup_node.childNodes:
            if itemdefinitiongroup_child_node.nodeType == itemdefinitiongroup_child_node.ELEMENT_NODE:
              if itemdefinitiongroup_child_node.nodeName == 'Link':
                link_nodes += [itemdefinitiongroup_child_node]
                break
          
    print('link nodes:', link_nodes)
    print('def value:', def_value)
    if len(link_nodes) != 0 and def_value != '':
      for link_node in link_nodes:
        link_node.appendChild(doc.createTextNode('\n      '))
        def_elem = doc.createElement('ModuleDefinitionFile')
        def_elem.setAttribute('Condition', "'$(Configuration)|$(Platform)'=='Debug|x64'")
        def_elem.appendChild(doc.createTextNode(def_value))
        link_node.appendChild(def_elem)
        link_node.appendChild(doc.createTextNode('\n    '))
    if is_custom:
      if is_clinclude:
        clinclude_itemgroup.appendChild(doc.createTextNode('\n  '))
        project_node.insertBefore(clinclude_itemgroup, custom_itemgroup)
      if is_clcompile:
        project_node.insertBefore(doc.createTextNode('\n  '), custom_itemgroup)
        clcompile_itemgroup.appendChild(doc.createTextNode('\n  '))
        project_node.insertBefore(clcompile_itemgroup, custom_itemgroup)
      if is_resourcecompile:
        project_node.insertBefore(doc.createTextNode('\n  '), custom_itemgroup)
        resourcecompile_itemgroup.appendChild(doc.createTextNode('\n  '))
        project_node.insertBefore(resourcecompile_itemgroup, custom_itemgroup)
      project_node.removeChild(custom_itemgroup)
    try:
      if not os.path.exists(os.path.dirname(dest_xml_fname)):
        os.makedirs(os.path.dirname(dest_xml_fname))
      doc.writexml(open(dest_xml_fname, 'w'), indent="", addindent="",newl='',encoding="utf-8")
      doc.unlink()
    except Exception as err:
      print(type(err))
          
          
    

    
def change_vcxproj_filter_xml(src_xml_fname, dest_xml_fname): 
  is_custom = False
  is_clcompile = False
  is_clinclude = False
  is_resourcecompile = False
  custom_itemgroup = None
  clcompile_itemgroup = None
  clinclude_itemgroup = None
  resourcecompile_itemgroup = None
  doc = xml.dom.minidom.parse(src_xml_fname)
  print("doc = %s" %doc.nodeName)
  print("\tnode = %s" %doc.firstChild.tagName)
  if doc.firstChild.nodeName == 'Project':
    project_node = doc.firstChild
    project_child_nodes = project_node.childNodes    
    for project_child_node in project_child_nodes:
      if project_child_node.nodeType == project_child_node.ELEMENT_NODE:
        print("\t\tproject node = %s" %project_child_node.nodeName)
        if project_child_node.nodeName == 'ItemGroup':
          itemgroup_node = project_child_node
          itemgroup_child_nodes = itemgroup_node.childNodes
          cl_include_values = []
          for itemgroup_child_node in itemgroup_child_nodes:
            if itemgroup_child_node.nodeType == itemgroup_child_node.ELEMENT_NODE:
              print("\t\t\t itemgroup node = %s" %itemgroup_child_node.nodeName)
              if itemgroup_child_node.nodeName == 'CustomBuild':
                if not is_custom:
                  custom_itemgroup = itemgroup_node
                  is_custom = True
                if not is_clcompile:
                  clcompile_itemgroup = doc.createElement('ItemGroup')
                  is_clcompile = True
                include_value = itemgroup_child_node.getAttribute('Include')
                if include_value not in cl_include_values:
                  cl_include_values.append(include_value)
                  clcompile_itemgroup.appendChild(doc.createTextNode('\n    '))
                  print("\t\t\t\tinclude value = %s" %include_value)
                  cl_elem = doc.createElement('ClCompile')
                  cl_elem.setAttribute('Include', include_value)
                  custombuild_child_nodes = itemgroup_child_node.childNodes
                  for custombuild_child_node in custombuild_child_nodes:
                    if custombuild_child_node.nodeType == custombuild_child_node.ELEMENT_NODE:
                      if custombuild_child_node.nodeName == 'Filter':
                        filter_node = custombuild_child_node
                        if filter_node.firstChild:
                          filter_value = filter_node.firstChild.nodeValue
                          print("\t\t\t\t\tfilter value = %s" %filter_value)
                          cl_elem.appendChild(doc.createTextNode('\n      '))
                          filter_elem = doc.createElement('Filter')
                          filter_elem.appendChild(doc.createTextNode(filter_value))
                          cl_elem.appendChild(filter_elem)
                          cl_elem.appendChild(doc.createTextNode('\n    '))
                  clcompile_itemgroup.appendChild(cl_elem)
              if itemgroup_child_node.nodeName == 'None':
                if not is_custom:
                  custom_itemgroup = itemgroup_node
                  is_custom = True
                if not is_clinclude:
                  clinclude_itemgroup = doc.createElement('ItemGroup')
                  is_clinclude = True
                include_value = itemgroup_child_node.getAttribute('Include')
                if include_value not in cl_include_values:
                  cl_include_values.append(include_value)
                  include_value = itemgroup_child_node.getAttribute('Include')
                  include_value_ext = include_value.rsplit('.', 1)[1]
                  if include_value_ext == 'rc':
                    if not is_resourcecompile:
                      resourcecompile_itemgroup = doc.createElement('ItemGroup')
                      is_resourcecompile = True
                    resourcecompile_itemgroup.appendChild(doc.createTextNode('\n    '))
                    rc_elem = doc.createElement('ResourceCompile') 
                    rc_elem.setAttribute('Include', include_value)
                    resourcecompile_itemgroup.appendChild(rc_elem) 
                    continue
                  clinclude_itemgroup.appendChild(doc.createTextNode('\n    '))
                  print("\t\t\t\tinclude value = %s" %include_value)
                  cl_elem = doc.createElement('ClInclude')
                  cl_elem.setAttribute('Include', include_value)
                  none_child_nodes = itemgroup_child_node.childNodes
                  for none_child_node in none_child_nodes:
                    if none_child_node.nodeType == none_child_node.ELEMENT_NODE:
                      if none_child_node.nodeName == 'Filter':
                        filter_node = none_child_node
                        if filter_node.firstChild:
                          filter_value = filter_node.firstChild.nodeValue
                          print("\t\t\t\t\tfilter value = %s" %filter_value)
                          cl_elem.appendChild(doc.createTextNode('\n      '))
                          filter_elem = doc.createElement('Filter')
                          filter_elem.appendChild(doc.createTextNode(filter_value))
                          cl_elem.appendChild(filter_elem)
                          cl_elem.appendChild(doc.createTextNode('\n    '))                      
                  clinclude_itemgroup.appendChild(cl_elem)
          
    if is_custom:
      if is_clinclude:
        clinclude_itemgroup.appendChild(doc.createTextNode('\n  '))
        project_node.insertBefore(clinclude_itemgroup, custom_itemgroup)
      if is_clcompile:
        project_node.insertBefore(doc.createTextNode('\n  '), custom_itemgroup)
        clcompile_itemgroup.appendChild(doc.createTextNode('\n  '))
        project_node.insertBefore(clcompile_itemgroup, custom_itemgroup)
      if is_resourcecompile:
        project_node.insertBefore(doc.createTextNode('\n  '), custom_itemgroup)
        resourcecompile_itemgroup.appendChild(doc.createTextNode('\n  '))
        project_node.insertBefore(resourcecompile_itemgroup, custom_itemgroup)
      project_node.removeChild(custom_itemgroup)      
    try:
      if not os.path.exists(os.path.dirname(dest_xml_fname)):
        os.makedirs(os.path.dirname(dest_xml_fname))
      doc.writexml(open(dest_xml_fname, 'w'), indent="", addindent="",newl='',encoding="utf-8")
      doc.unlink()
    except Exception as err:
      print(type(err))        
    
def traverse_directory(srcdir, destdir):
  print "traverse directory ..."

  for root,d_names,f_names in os.walk(srcdir):
    # print "line: ", root, d_names, f_names
    for f in f_names:
      src_fname = os.path.join(root, f)
      rel_fname = src_fname.replace(srcdir, '').lstrip('\\/')
      dest_fname = os.path.join(destdir, rel_fname)
      #print("src fname  = %s" %src_fname)
      #print("dest fname = %s" %dest_fname)
      #print("rel fname  = %s" %rel_fname)
      if rel_fname.endswith('.vcxproj'):
        #print("src fname  = %s" %src_fname)
        #print("dest fname = %s" %dest_fname)
        change_vcxproj_xml(src_fname, dest_fname)
        
      if rel_fname.endswith('.vcxproj.filters'):
        #print("src fname  = %s" %src_fname)
        #print("dest fname = %s" %dest_fname)
        change_vcxproj_filter_xml(src_fname, dest_fname)
        
        
def main():
  print "msvc project tools ..."
  parser = ArgumentParser()
  parser.add_argument("-src", "--srcdir", help='source directory for ninja msvc project', default='')
  parser.add_argument("-dest", "--destdir", help='dest directory for non-ninja msvc project', default='')

  args = parser.parse_args()
  if args.srcdir == '' and args.destdir == '':
    print 'need input -src and -dest'
  else:  
    traverse_directory(args.srcdir, args.destdir)

if __name__== "__main__":
  main()