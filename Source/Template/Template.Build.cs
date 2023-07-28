using UnrealBuildTool;

public class Template : ModuleRules
{
	public Template(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "EnhancedInput", "AIModule", "Niagara" });
    }
}