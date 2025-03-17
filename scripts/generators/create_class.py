import getopt
import sys
import getpass
import os
import datetime

header_code = ("\n"
               "#pragma once\n"
               "\n"
               "{disclaimer}\n"
               "{namespace_open}\n"
               "class {name}\n"
               "{{\n"
               "}};\n"
               "\n"
               "{namespace_close}\n")

source_code = ("\n"
               "#include <{dir}/{name}.h>\n"
               "\n"
               "{disclaimer}\n"
               "{namespace_open}\n"
               "\n"
               "{namespace_close}\n")

test_code = ("\n"
             "#include <{dir}/{name}.h>\n"
             "#include <gtest/gtest.h>\n"
             "\n"
             "{disclaimer}\n"
             "{namespace_open}namespace Tests\n"
             "{{\n"
             "struct {name}Test : testing::Test\n"
             "{{\n"
             "}};\n\n"
             "}}{namespace_close}\n")


def main(argv):
    classname = ''
    dirname = ''
    namespace = ''
    packagename = 'pardal-core'
    createtests = False
    createimplementation = True
    show_help = False
    banner = 'create_class.py -c classname -d dirname [-i -t -n namespace -p packagename]'

    try:
        opts, args = getopt.getopt(argv, "hc:d:itn:p:", ["class=", "directory=", "namespace=", "packagename="])
    except getopt.GetoptError:
        print(banner)
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            show_help = True
        elif opt in ("-c", "--class"):
            classname = arg
        elif opt in ("-d", "--directory"):
            dirname = arg
        elif opt in ("-n", "--namespace"):
            namespace = arg
        elif opt in ("-p", "--packagename"):
            packagename = arg
        elif opt == '-i':
            createimplementation = False
            createtests = False
        elif opt == '-t':
            createtests = True

    if classname == '' or dirname == '' or show_help:
        print(banner)
        print("""
            -h:             Displays this help
            -c classname:   (REQUIRED) Sets the specified class name 
            -d dirname:     (REQUIRED) Saves the files in specified subfolder
            -i:             Creates header only
            -t:             Adds a test for that class
            -n namespace:   Adds the class inside the specified namespace
            -p packagename: Creates class in specified package (default is 'pardal-core')
        """)
        sys.exit()

    opennamespace = ("\n"
                     "namespace pdl\n"
                     "{\n")
    closenamespace = "}\n"

    now = datetime.datetime.now()
    disclaimer = "// Created on {0} by {1}".format(now.date(), getpass.getuser())

    if namespace != '':
        opennamespace += "namespace {namespace}\n{\n".format(namespace=namespace)
        closenamespace = "}\n" + closenamespace

    header_path = "packages/{packagename}/{dir}/{name}.h".format(dir=dirname, name=classname, packagename=packagename)
    print('Creating header in {0}'.format(header_path))
    os.makedirs(os.path.dirname(header_path), exist_ok=True)
    with open(header_path, "w") as f:
        f.write(header_code.format(namespace_open=opennamespace,
                                   name=classname,
                                   namespace_close=closenamespace,
                                   disclaimer=disclaimer))

    if createimplementation:
        source_path = "packages/{packagename}/{dir}/{name}.cpp".format(dir=dirname, name=classname, packagename=packagename)
        print('Writing source to {0}'.format(source_path))
        os.makedirs(os.path.dirname(source_path), exist_ok=True)
        with open(source_path, "w") as f:
            f.write(source_code.format(namespace_open=opennamespace,
                                       name=classname,
                                       namespace_close=closenamespace,
                                       dir=dirname,
                                       disclaimer=disclaimer))

    if createtests:
        test_path = "packages/pardal-tests/{dir}/{name}Test.cpp".format(dir=dirname, name=classname)
        print('Creating test file in {0}'.format(test_path))
        os.makedirs(os.path.dirname(test_path), exist_ok=True)
        with open(test_path, "w") as f:
            f.write(test_code.format(namespace_open=opennamespace,
                                     name=classname,
                                     namespace_close=closenamespace,
                                     dir=dirname,
                                     disclaimer=disclaimer))


if __name__ == "__main__":
    main(sys.argv[1:])
