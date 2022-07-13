using UnrealBuildTool;

public class ScratchEditorTarget : TargetRules
{
    public ScratchEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.AddRange( new string[] { "ScratchCore" } );
    }
}
