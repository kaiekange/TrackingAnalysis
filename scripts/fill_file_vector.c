#include <vector>
#include <iostream>
#include <glob.h>
#include <TString.h>
vector<TString> fill_file_vector( string path_str ) {
    glob_t glob_result;
    glob(path_str.c_str(), GLOB_TILDE, nullptr, &glob_result);

    vector<TString> file_vector;
    for(size_t i=0; i<glob_result.gl_pathc; i++){
        file_vector.push_back(glob_result.gl_pathv[i]);
    }
    globfree(&glob_result);
    return file_vector;
}
