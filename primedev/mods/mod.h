class Mod;

struct ModConVar
{
public:
	std::string Name;
	std::string DefaultValue;
	std::string HelpString;
	int Flags;
};

struct ModConCommand
{
public:
	std::string Name;
	std::string Function;
	std::string HelpString;
	ScriptContext Context;
	int Flags;
};

struct ModScriptCallback
{
public:
	ScriptContext Context;

	// called before the codecallback is executed
	std::string BeforeCallback;
	// called after the codecallback has finished executing
	std::string AfterCallback;
	// called right before the vm is destroyed.
	std::string DestroyCallback;
};

struct ModScript
{
public:
	std::string Path;
	std::string RunOn;

	std::vector<ModScriptCallback> Callbacks;
};

// these are pretty much identical, could refactor to use the same stuff?
struct ModVPKEntry
{
public:
	bool m_bAutoLoad;
	std::string m_sVpkPath;
};

struct ModRpakEntry
{
public:
	ModRpakEntry(Mod& parent)
		: m_parent(parent)
		, m_loadRegex("^thisMatchesNothing^") // discord couldnt give me a funny string
	{
	}

	Mod& m_parent;
	std::string m_pakName;
	std::regex m_loadRegex;

	// these exist purely for backwards compatibility, i don't really like them anymore

	// Preload, loads before the first rpak is loaded
	bool m_preload = false;
	// Postload, this rpak depends on an rpak with this hash
	size_t m_dependentPakHash;
};

class Mod
{
public:
	// runtime stuff
	bool m_bEnabled = true;
	bool m_bWasReadSuccessfully = false;
	fs::path m_ModDirectory;
	bool m_bIsRemote;

	// mod.json stuff:

	// the mod's name
	std::string Name;
	// the mod's description
	std::string Description;
	// the mod's version, should be in semver
	std::string Version;
	// a download link to the mod, for clients that try to join without the mod
	std::string DownloadLink;

	// whether clients need the mod to join servers running this mod
	bool RequiredOnClient;
	// the priority for this mod's files, mods with prio 0 are loaded first, then 1, then 2, etc
	int LoadPriority;

	// custom scripts used by the mod
	std::vector<ModScript> Scripts;
	// convars created by the mod
	std::vector<ModConVar*> ConVars;
	// concommands created by the mod
	std::vector<ModConCommand*> ConCommands;
	// custom localisation files created by the mod
	std::vector<std::string> LocalisationFiles;
	// custom script init.nut
	std::string initScript;

	// other files:

	std::vector<ModVPKEntry> Vpks;
	std::unordered_map<size_t, std::string> KeyValues;
	std::vector<std::string> BinkVideos;
	std::string Pdiff; // only need one per mod

	std::vector<ModRpakEntry> Rpaks;
	// paks we alias to other rpaks, e.g. to load sp_crashsite paks on the map mp_crashsite
	std::unordered_map<std::string, std::string> RpakAliases;
	// starpaks that this mod contains
	// there seems to be no nice way to get the rpak that is causing the load of a starpak?
	// hashed with STR_HASH
	std::vector<size_t> StarpakPaths;

	std::unordered_map<std::string, std::string> DependencyConstants;
	std::vector<std::string> PluginDependencyConstants;

public:
	Mod(fs::path modPath, const char* jsonBuf);

private:
	void ParseConVars(rapidjson_document& json);
	void ParseConCommands(rapidjson_document& json);
	void ParseScripts(rapidjson_document& json);
	void ParseLocalization(rapidjson_document& json);
	void ParseDependencies(rapidjson_document& json);
	void ParsePluginDependencies(rapidjson_document& json);
	void ParseInitScript(rapidjson_document& json);
};
