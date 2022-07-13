using UnrealBuildTool;

public class ScratchTarget : TargetRules {
    public ScratchTarget(TargetInfo Target) : base(Target) {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.AddRange( new string[] { "ScratchCore" } );
    }
}
