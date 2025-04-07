import os


def build_project():
    wd = os.path.abspath(os.path.join(os.getcwd() + "/../../"))

    os.chdir(wd + "/cosima_omnetpp_project")
    os.system("make -f makemakefiles")
    os.system("make")


build_project()
