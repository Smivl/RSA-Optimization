#include <iostream>
#include <string>
#include <iomanip>
#include <thread>
#include <map>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include "bignum.hpp"

constexpr int MAX_THREAD_COUNT = 16;

const Bignum rsa_n = Bignum("9616540267013058477253762977293425063379243458473593816900454019721117570003248808113992652836857529658675570356835067184715201230519907361653795328462699");
const Bignum rsa_e = Bignum("65537");
const Bignum rsa_d = Bignum("4802033916387221748426181350914821072434641827090144975386182740274856853318276518446521844642275539818092186650425384826827514552122318308590929813048801");


int line_count = 1;

std::mutex globalMutex;
std::condition_variable cv;
int active_threads = 0;

std::map<int, std::string> lines;


void printUsage() {
    std::cout << "Usage: ./bignum <mode> <string>" << std::endl;
}

std::string lineCountPrepend(){
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill(' ') << line_count++;
    return oss.str();  
}

std::string convertStringToStrNums(const std::string& str){
    std::string res;
    for(auto& character : str){
        std::ostringstream oss;
        oss << std::setw(3) << std::setfill('0') << int(character);
        res += oss.str();
    }
    return std::move(res);
}

void removeTrailingBlanks(std::string& str) {
    auto it = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    });

    str.erase(it.base(), str.end());
}

std::string encrypt_string(std::string& m){
    Bignum msg = Bignum(convertStringToStrNums(m));
    return std::move(msg.modexp(rsa_e, rsa_n).to_string());
}

std::string decrypt_string(std::string& m){
    Bignum msg(m);
    return std::move(msg.modexp(rsa_d, rsa_n).fromEncrypted());
}

void encrypt_thread(int id, std::string line){
    // Get the text length and line number
    int textLength = line.length();
    std::string presuffix = lineCountPrepend();

    // prefix to text and account for it in textLength
    line = presuffix + line;
    textLength += 3;

    // Divide into two 51 character chunks and add suffix
    int spaces = 102 - textLength - 3;
    if (spaces > 0) {
        line += std::string(spaces, ' ');
        //std::cout << line.size() << std::endl;
        
    }
    else if(spaces < 0) {
        line = line.substr(0, 99);
        //std::cout << line.size() << std::endl;
    }
    line += presuffix;
    //std::cout << line.size() << std::endl;
    // Split into 2 chunks
    std::string p1 = line.substr(0, 51);
    std::string p2 = line.substr(51);

    // Encrypt and print
    std::string e1_msg = encrypt_string(p1);
    std::string e2_msg = encrypt_string(p2);

    std::unique_lock<std::mutex> ulck(globalMutex);
    lines.emplace(id, e1_msg + "\n" + e2_msg);
    --active_threads;
    cv.notify_one();
}

void encrypt(){

    std::string line;
    int line_number = 1;
    std::vector<std::thread> threads;

    while(std::getline(std::cin, line)){

        std::unique_lock<std::mutex> lock(globalMutex);
        cv.wait(lock, [](){ return active_threads < MAX_THREAD_COUNT; });
        active_threads++;
        threads.emplace_back(encrypt_thread, line_number++, line);

        
    }

    for(auto& thread : threads){
        thread.join();
    }
    for(auto& pair : lines){
        std::cout << pair.second << std::endl;
    }
}

void decrypt_thread(int id, std::string l1, std::string l2){
    std::string d1_msg = decrypt_string(l1);
    std::string d2_msg = decrypt_string(l2);
    
    std::string result = d1_msg + d2_msg;
    result = result.substr(3);
    result = result.substr(0,result.length()-3);
    removeTrailingBlanks(result);

    std::unique_lock<std::mutex> ulck(globalMutex);
    lines.emplace(id, result);
    --active_threads;
    cv.notify_one();
}

void decrypt(){

    std::string line1;
    std::string line2;

    int line_number = 1;

    std::vector<std::thread> threads;

    while(std::getline(std::cin, line1) && std::getline(std::cin, line2)){
        
        std::unique_lock<std::mutex> lock(globalMutex);
        cv.wait(lock, [](){ return active_threads < MAX_THREAD_COUNT; });
        active_threads++;
        threads.emplace_back(decrypt_thread, line_number++, line1, line2);
    }

    for(auto& thread : threads){
        thread.join();
    }
    for(auto& pair : lines){
        std::cout << pair.second << std::endl;
    }
}

int main(int argc, char* argv[]) {
    
    // Get arguments
    if (argc < 2) {
        printUsage();
        return 1;
    }  
    char mode = *argv[1];

    switch (mode){
    case 'e':
        encrypt();
        break;
    
    case 'd':
        decrypt();
        break;
    default:
        std::cerr << "use 'e' for encryption or 'd' for decryption!" << std::endl;
        return 1;
    }
    
    return 0;
}
