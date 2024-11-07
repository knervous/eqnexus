export module config;

import hooks;
import globals;
import filesystem;
import server;
import zones;
import animations;
import npcs;
import options;

import <string>;
import <iostream>;
import <fstream>;
import <rapidjson/document.h>;

export class Config {

public:
    static void Init() {
        Server::RegisterCallback("Config", OnServerContextChanged);
    }

    static void Teardown() {
        Server::RemoveCallback("Config");
    }

private:
    static void OnServerContextChanged(const std::string& context) {
        if (context.empty()) {
            RemoveHooks();
        }
        else {
            SetupHooks();
        }
    }

    static void SetupHooks() {
        auto path = "eqnexus/" + Server::GetContext() + "/config.json";

        std::ifstream file(path);
        if (!file) {
            return;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        rapidjson::Document document;
        document.Parse(content.c_str());

        if (document.HasParseError()) {
            return;
        }

        // Zones
        if (document.HasMember("zones") && document["zones"].IsObject()) {
            ZoneConfig::Init(document["zones"].GetObj());
        }

        // Animations
        if (document.HasMember("animations") && document["animations"].IsObject()) {
            AnimationConfig::Init(document["animations"].GetObj());
        }

        // NPCs
        if (document.HasMember("npcs") && document["npcs"].IsArray()) {
            NpcConfig::Init(document["npcs"].GetArray());
        }

        // Options
        if (document.HasMember("options") && document["options"].IsObject()) {
            OptionsConfig::Init(document["options"].GetObj());
        }
    }

    static void RemoveHooks() {
        ZoneConfig::Teardown();
        AnimationConfig::Teardown();
        NpcConfig::Teardown();
        OptionsConfig::Teardown();
    }
};
