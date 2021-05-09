#include "../uWebSockets/src/App.h"
#include "pugixml/src/pugixml.hpp"
#include "nlohmann/json.hpp"
#include <pcrecpp.h>

#include "../uWebSockets/src/Multipart.h"
#include <iostream>
#include <iterator>
#include <string>
#include <regex>
#include <fstream>
#include <filesystem>
#include <sys/types.h>
#include <math.h>
#include <chrono>
#include <sys/stat.h>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "base64/base64.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

struct stat info;
std::string nth_package = "0";

pugi::xml_document document;
pugi::xml_parse_result doc = document.load_file("tree-pack.xml");

bool check_ID(std::string ID) {
    fs::path p = fs::current_path().u8string();
    p += "\\packages\\" + ID;
    std::string pathStr = (std::string)(p);
    std::string filesInPath;
    for (const auto& file : fs::directory_iterator(((std::string)fs::current_path() + "/packages/")))
        filesInPath += file.path().string() + "\n";
    std::cout << "reached check: " << (bool)(filesInPath.find(ID) != std::string::npos) << std::endl;
    //TODO: Check tree-pack.xml to see if any packages with the ID exist.

    //oh fuck I commented this line as bookmark before going to sleep out of frustration because I was unable to fix something but I can't remember what needed to be fixed, everything seems to be working and now I'm scared.
    std::cout << "Boolcheck: " << (bool)(filesInPath.find(ID) != std::string::npos) << "\nChecking for existent path in directory: " << (bool)(filesInPath.find("hallo") != std::string::npos) << std::endl;

    return (bool)(filesInPath.find(ID) != std::string::npos);
}

std::string generate_ID() {
    std::string temp_seed;
    std::string predefinedSeed;
    std::fstream seedFile("seed.txt");
    std::fstream packageFile("package.txt");

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    uint64_t Seed;
    unsigned long long unique_ishNum = (float)((1<<64) / (ms.count())) * (10000);


    if (seedFile.is_open()) {
        std::getline(seedFile, predefinedSeed);
        seedFile.close();
    }
    if (packageFile.is_open()) {
        std::getline(packageFile, nth_package);
        packageFile.close();
    }

    if (predefinedSeed == "") {
        std::cout << "Decimal MAC address " << unique_ishNum << std::endl;
        std::cout << "Unix epoch time " << time(NULL) << std::endl;
        Seed = (time(NULL) + unique_ishNum) % 18446744073709551615;
        std::ofstream seedFile("seed.txt");
        if (seedFile.is_open()) {
            seedFile << std::to_string(Seed);
        }
    } else {
        Seed = std::stoull(predefinedSeed);
    }
    srand(Seed);
    unsigned long long ID = rand();

    std::cout << nth_package << std::endl;
    int nth_package_int = std::stoi(nth_package);
    ID *= nth_package_int;
    std::string integer = std::to_string(ID);
    std::cout << "int: " << integer << std::endl;

    nth_package = std::to_string(++nth_package_int);
    std::cout << "h: " << nth_package_int << ", " << nth_package << std::endl;
    std::ofstream packageIndex("package.txt");

    if (packageIndex.is_open())
        packageIndex << nth_package;

    if (!check_ID(integer)) return integer;
    else generate_ID();
}

int main() {
    uWS::App().get("/get/:name", [](auto *res, auto *req) {
        try {
            res->onAborted([=]() {
                std::cout << "what." << std::endl; 
            });
            if (req->getParameter(0) == "all") {
                json response;
                pugi::xml_node nodes = document.child("Packages");

                for (pugi::xml_node node : nodes.children()) {
                    response["all_packages"] += node.attribute("name").value();
                }

                return res->writeHeader("Content-Type", "application/json; charset=utf-8")->end(response.dump());
            } else {
                std::string packageName{req->getParameter(0)};
                if (packageName == "") return res->end("Invalid package name!");
                std::string result;
                int ii;
                for (int i=0; i<packageName.length();i++) {
                    if (int(packageName[i]) == 37) {
                        sscanf(packageName.substr(i+1, 2).c_str(), "%x", &ii); //FIXME: Research vulnerabilities and patches to those. XSS to start.
                        result += (char)(ii);
                        i += 2;
                    } else {
                        result += packageName[i];
                    }
                }

                pugi::xml_document document;
                pugi::xml_parse_result doc = document.load_file("tree-pack.xml");

                if (!doc)
                throw std::invalid_argument("Cannot load document"); //TODO: Add error handler to hijack webfeeds for error handling

                pugi::xml_node ultimateParent = document.child("Packages");
                pugi::xml_node soughtPackage = ultimateParent.find_child_by_attribute("Individual", "name", packageName.c_str());

                if (!soughtPackage)
                    return res->end("I can't find a package under that name!");//throw std::invalid_argument("Invalid package"); //TODO: Implement correct response, like autocorrect for suggesting packages with similar names

                json obj;
                std::string beautyArray[8] = {"name", soughtPackage.child("Name").child_value(), "description", soughtPackage.child("Description").child_value(), "ID", soughtPackage.child("ID").child_value(), "file path", soughtPackage.child("FilePath").child_value()};

                for (long unsigned int i=0;i<=(sizeof(beautyArray)/sizeof(*beautyArray) -1);i++) {
                    if (i%2==0) {
                        std::cout << beautyArray[i] << ": " << beautyArray[i+1] << std::endl;
                        obj[beautyArray[i]] = beautyArray[i+1];
                    } else {
                        continue;
                    }
                }
                std::cout << "Opening file" << std::endl;
                std::ifstream FileData("./packages/" + (std::string)(soughtPackage.child("ID").child_value()) + "/main.zip");
                std::stringstream PackageContent;
                std::string line;

                if (FileData.is_open()) {
                    std::cout << "Opened file" << std::endl;
                    while (getline(FileData, line)) {
                        std::vector<char> lineVector(line.begin(), line.end());
                        PackageContent << base64::encode(lineVector);
                    }
                    FileData.close();
                    std::cout << "Closed file" << std::endl;

                    obj["file"] = PackageContent.str();
                    std::string objectString = obj.dump();
                    std::cout << objectString << std::endl;
                    res->writeHeader("Content-Type", "application/json; charset=utf-8")->end(objectString);
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Err: " << e.what();
        }
    }).post("/give", [](auto *res, auto *req) {
        const uint64_t contentLength = std::stoi(std::string(req->getHeader("content-length")));
        json headerObject;

        //std::cout << uWS::ParameterParser::getKeyValue().first << uWS::ParameterParser::getKeyValue().second << std::endl;

        for (std::pair header : *req) {
            if (header.first == "content-type") {
                std::regex reg("multipart\\/form-data;\\ boundary=(-*)(.*)");
                if (!(std::regex_search(std::string{header.second}, reg))) return res->end("Invalid headers. Please send your package with all required keys as form-data."); //TODO: Error handler struct
            }
            headerObject[std::string{header.first}] = header.second;
        }

        std::string buffer;
        std::string dataStr;
        json formdata;
        std::string ID = generate_ID();

        res->onData([contentLength, res, dataStr, buffer, formdata, ID](std::string_view data, bool last) mutable {
            std::cout << "Formdata " << last << " " << data.data();
            std::ofstream log("log.txt");
            std::string dataString{data.data()};
            log << data.data();
            log.close();
            if (last) {
                try {
                    std::string formData{data.data()};
                    res->onAborted([=]() {
                        std::cout << "I don't even fucking know what's going on but apparently this tricks uWebSockets to think we handled an abortion while in reality I'm a dude so it's hard for me to abort something." << std::endl; 
                    });

                    std::string match;
                    pcrecpp::RE reg("-+.+\\nContent-Disposition: form-data; name=\"(\\w+| +)\"; filename=\"(.+)\"\\nContent-Type: (\\w+\\/\\w+)\\n\\n((.|\\n)+)\\n-+.+--.+|\\n+");
                    pcrecpp::StringPiece input(data.data());

                    for (int i=0;i<5;i++) {
                        reg.FindAndConsume(&input, &match);
                        std::cout << "regex match " << i << ": " << match << std::endl;
                    }
                    std::cout << "Hopefully there's an error: " << reg.error() << std::endl;
                    

                    
                    /* if (std::regex_search(formData, match, std::regex("-++.+\\nContent-Disposition: form-data; name=\"(\\w+|\\ +)+\"; filename=\"(.+)\"\\nContent-Type: (\\w+\\/\\w+\\n)\\n((.|\\n)+)\\n-+.+--.+|\\n+", std::regex_constants::basic))) {
                        std::cout << "got regex" << std::endl;
                        std::cout << "Things" << match[0] << std::endl;
                        formdata["Name"] = match[1];
                        formdata["FileName"] = match[2];
                        formdata["Content-Type"] = match[3];
                        formdata["FileData"] = match[4];

                        std::cout << "Formdata: " << formdata.dump();

                        if (formdata["Content-Type"] != "application/zip") res->end("Please send your package in ZIP format.");

                        std::cout << "Your file is " << formdata["FileName"] << ". Of type " << formdata["Content-Type"] << std::endl;
                        std::cout << "Under the variable name of " << formdata["Name"] << std::endl;
                        std::string path = std::string(fs::current_path()) + "/packages/" + ID + "/";
                        fs::create_directory(std::string(fs::current_path()) + "/packages/", ID);

                        std::ofstream fileToWrite(path + formdata["FileName"].dump());
                        if (fileToWrite.is_open())
                            fileToWrite << formdata["FileData"];
                        fileToWrite.close();
                    } else if (std::regex_search(dataString, match, std::regex("Content-Disposition:\\ form-data;\\ name=\\\"(\\w+|\\ +)+\\\";\\nContent-Type: (\\w+\\/\\w+\\n)\\n((.|\\n)+)\\n-+.+--.+|\\n+"))) {
                        std::cout << "No file, only value." << std::endl;
                        formdata["Name"] = match[1];
                    } else {
                        std::cout << "what the fuck did you send?" << std::endl;
                        //return res->end("Your formdata is invalid.");
                    } */

                    pugi::xml_node newChild = document.child("Packages").append_child("Indvidual");
                    newChild.append_attribute("name") = "name";
                    newChild.append_child("Name").append_child(pugi::node_pcdata).set_value("name");
                    newChild.append_child("Description").append_child(pugi::node_pcdata).set_value("description");
                    newChild.append_child("ID").append_child(pugi::node_pcdata).set_value(ID.c_str());
                    newChild.append_child("FilePath").append_child(pugi::node_pcdata).set_value(("/packages/" + ID).c_str());
                    document.save_file("tree-pack.xml");

                    std::cout << "Name " << formdata["Name"] << std::endl;
                    std::cout << "buff " << buffer << data.data() << std::endl;

                    res->end("received.");
                } catch (const std::exception& e) {
                    std::cout << "Err: " << e.what();
                }
            } else {
                if (dataStr.length() == 0) {
                    dataStr.reserve(contentLength);
                }
                dataStr.append(data.data(), data.size());
            }
        });
    }).listen(3000, [](auto *listen_socket) {
        if (listen_socket) {
            std::cout << "Listening on port " << 3000 << std::endl;
        }
    }).run();

    std::cout << "Failed to listen on port 3000" << std::endl;
}