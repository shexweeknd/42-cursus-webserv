#include "urlParser.hpp"

std::map<std::string, std::string>  parseHttpRequest(const std::string& request)
{
    std::map<std::string, std::string>  headers;
    std::istringstream                  iss(request);
    std::string                         line;

    // Traiter la première ligne (GET /favicon.ico HTTP/1.1)
    std::getline(iss, line);
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    if (line.empty()) return (headers); // Pas de requête
    std::string method, path, httpVersion;
    std::istringstream firstLine(line);
    firstLine >> method >> path >> httpVersion;
    if (method.empty() || path.empty() || httpVersion.empty()) {
        std::cerr << "\e[31mInvalid request line: \e[0m" << line << std::endl;
        return (headers);
    }
    // Vérifier la méthode
    if (method != "GET" && method != "POST" && method != "PUT" && method != "DELETE") {
        std::cerr << "\e[31mUnsupported HTTP method: \e[0m" << method << std::endl;
        return (headers);
    }
    // Vérifier la version HTTP
    if (httpVersion != "HTTP/1.0" && httpVersion != "HTTP/1.1") {
        std::cerr << "\e[31mUnsupported HTTP version: \e[0m" << httpVersion << std::endl;
        return (headers);
    }
    headers["method"] = method;
    headers["path"] = path;
    headers["httpVersion"] = httpVersion;

    // Parser les en-têtes
    while (std::getline(iss, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if (line.empty()) break; // Fin des en-têtes

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Supprimer les espaces en début/fin
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            headers[key] = value;
        }
    }

    return (headers);
}