import os


def build_project(path_to_omnet="/home/user/Downloads/omnetpp-5.6.2"):
    cwd = os.getcwd()
    os.chdir(path_to_omnet)
    os.system("echo $-; shopt login_shell")
    os.system(". /home/efrost/Downloads/omnetpp-5.6.2/setenv")
    os.chdir(cwd + "/mosaik_omnetpp_observer")
    os.system("make -f makemakefiles")
    os.system("make")
