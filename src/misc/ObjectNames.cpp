#include "ObjectNames.hpp"

void ObjectNames::checkNames() {
    auto req = web::WebRequest();

    m_listener.spawn(
        req.get("https://raw.githubusercontent.com/Alphalaneous/GDObjectNames/refs/heads/main/version"),
        [this] (web::WebResponse value) {
            if (value.error()) {
                loadNamesFromFile();
                return;
            }

            auto strRes = value.string();
            if (!strRes) {
                loadNamesFromFile();
                return;
            }

            auto numRes = numFromString<unsigned int>(strRes.unwrap());
            if (!numRes) {
                loadNamesFromFile();
                return;
            }

            auto version = numRes.unwrap();

            if (version > checkVersion()) {
                Mod::get()->setSavedValue("object-names-version", version);
                downloadNames();
            }
            else {
                loadNamesFromFile();
            }
        }
    );
}

void ObjectNames::loadNames(std::string_view names) {
    auto split = utils::string::split(names, "\n");
    for (const auto& str : split) {
        auto commaSplit = utils::string::split(str, ",");
        if (commaSplit.size() != 2) continue;

        auto idRes = numFromString<unsigned int>(commaSplit[0]);
        if (!idRes) continue;

        m_names[idRes.unwrap()] = commaSplit[1];
    }
}

void ObjectNames::loadNamesFromFile() {
    std::filesystem::path objectNames = Mod::get()->getSaveDir() / "objects.csv";
    if (!std::filesystem::exists(objectNames)) {
        downloadNames();
        return;
    }

    auto namesRes = utils::file::readString(objectNames);
    if (!namesRes) {
        downloadNames();
        return;
    }

    loadNames(namesRes.unwrap());
}

void ObjectNames::downloadNames() {
    auto req = web::WebRequest();
    
    m_listener.spawn(
        req.get("https://raw.githubusercontent.com/Alphalaneous/GDObjectNames/refs/heads/main/objects.csv"),
        [this] (web::WebResponse value) {
            if (value.error()) {
                std::filesystem::path objectNames = Mod::get()->getSaveDir() / "objects.csv";
                if (!std::filesystem::exists(objectNames)) return;
                
                auto namesRes = utils::file::readString(objectNames);
                if (!namesRes) return;

                loadNames(namesRes.unwrap());
                return;
            }

            auto strRes = value.string();
            if (!strRes) return;

            std::filesystem::path objectNames = Mod::get()->getSaveDir() / "objects.csv";
            auto writeRes = utils::file::writeString(objectNames, strRes.unwrap());
            if (!writeRes) {
                log::error("Failed to write object names");
            }
            
            loadNames(strRes.unwrap());
        }
    );
}

Result<std::string_view> ObjectNames::getName(unsigned int id) {
    auto iter = m_names.find(id);
    if (iter == m_names.end()) return Err("Name not found for ID");

    return Ok(std::string_view(iter->second));
}

const std::unordered_map<unsigned int, std::string>& ObjectNames::getNames() {
    return m_names;
}

int ObjectNames::checkVersion() {
    return Mod::get()->getSavedValue<unsigned int>("object-names-version", 0);
}

$on_game(Loaded) {
    ObjectNames::get()->checkNames();
}