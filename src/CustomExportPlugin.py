
# -- Python Base Imports --
import os
import sys

# -- Maya modules --
import shiboken2
import maya.cmds as cmds
import maya.api.OpenMaya as om2
import maya.api.OpenMayaUI as omu2
import maya.api.OpenMayaAnim as oma2
import maya.api.OpenMayaRender as omr2


from PySide2.QtWidgets import QWidget

# -- Python 2 v 3 Fixes --
if sys.version_info[0] == 3:
    from importlib import reload

def maya_useNewAPI():
    pass


class PixelPlayblastCmd(om2.MPxCommand):
    
    COMMAND_NAME = "CaptureViewRender"
    
    def __init__(self):
        super(PixelPlayblastCmd, self).__init__()
        
    def doIt(self, arg_list):
        self._viewer = omu2.M3dView.active3dView()
        self._window = shiboken2.wrapInstance(self._viewer.widget(), QWidget)
        self._window_width = self._window.size().width()
        self._window_height = self._window.size().height()

        self._window.resize(1920, 1080)
        self._window.repaint(0, 0, 1920, 1080)
        
        
        self._viewer.refresh()
        path = "C:/Users/Animator/Desktop/Tests/"
        file_name = "output"
        start_frame = 1
        end_frame = 40
        
        for frame in range(start_frame, end_frame, 1):
            self._viewer.refresh()
            oma2.MAnimControl.setCurrentTime(om2.MTime(frame))
            file_path = os.path.join(path, "{}_{:04d}.png".format(file_name, frame))
            img = om2.MImage()
            self._viewer.readColorBuffer(img)
            img.writeToFile(file_path, "png")
            print("Finished: {} / {}".format(frame, end_frame))
            
        self._window.resize(self._window_width, self._window_height)
        self._window.repaint(0, 0, self._window_width, self._window_height)
        
        
        
    @classmethod
    def creator(cls):
        return PixelPlayblastCmd()
    
    
    @classmethod
    def create_syntax(cls):
        syntax = om2.MSyntax()
        
        return syntax


def initializePlugin(plugin):
    
    vendor = "Jaron Wilding"
    version = "1.0.0"
    
    plugin_fn = om2.MFnPlugin(plugin, vendor, version)
    try:
        plugin_fn.registerCommand(PixelPlayblastCmd.COMMAND_NAME, PixelPlayblastCmd.creator, PixelPlayblastCmd.create_syntax)
    except:
        om2.MGlobal.displayError("Failed to register command: {0}".format(PixelPlayblastCmd.COMMAND_NAME))
    
def uninitializePlugin(plugin):
    plugin_fn = om2.MFnPlugin(plugin)
    
    try:
        plugin_fn.deregisterCommand(PixelPlayblastCmd.COMMAND_NAME)
    except:
        om2.MGlobal.displayError("Failed to de-register command: {0}".format(PixelPlayblastCmd))


if __name__ == "__main__":
    cmds.file(new=True, force=True)
    
    plugin_name = "pzPixelPlayblastPlugin.py"
    cmds.evalDeferred('if cmds.pluginInfo("{0}", q=True, loaded=True): cmds.unloadPlugin("{0}")'.format(plugin_name))
    cmds.evalDeferred('if not cmds.pluginInfo("{0}", q=True, loaded=True): cmds.loadPlugin("{0}")'.format(plugin_name))