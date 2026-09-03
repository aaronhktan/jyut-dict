function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/Jyut Dictionary.exe", "@StartMenuDir@/Jyut Dictionary.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/icon.ico", "description=Jyut Dictionary");

        component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/Uninstall Jyut Dictionary.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/maintenancetool.exe", "description=Uninstall Jyut Dictionary");

        component.addElevatedOperation("Execute", "@TargetDir@\\rename.bat", "@StartMenuDir@" + "\\");
    }
}
