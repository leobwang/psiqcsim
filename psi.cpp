// psi.cpp

#include <iostream>
#include <complex>
#include <vector>
#include <algorithm>
#include "psi.h"    
using namespace std;

namespace psi{
    State::State(complex<double> vec[], int n): n(n), dim(1 << n){
        this->vec = new complex<double>[dim];
        for (int i = 0; i < dim; ++i){
            this->vec[i] = vec[i];
        }
    }
    State::State(const string& name){
        vector<string> unary_names{"+", "-", "i", "-i", "0", "1"};
        if (find(unary_names.begin(), unary_names.end(), name) != unary_names.end()){
            this->n = 1;
        }
        else{
            throw runtime_error("State(const string& name): Invalid state name");
        }
        this->dim = 1 << n;
        this->vec = new complex<double>[dim];
        for (int i = 0; i < this->dim; ++i){
            this->vec[i] = 0;
        }
        if (name == "+"){
            this->vec[0] = sqrt(1./2.);
            this->vec[1] = sqrt(1./2.);
        }
        else if (name == "-"){
            this->vec[0] = sqrt(1./2.);
            this->vec[1] = -sqrt(1./2.);
        }
        else if (name == "i"){
            this->vec[0] = sqrt(1./2.);
            this->vec[1] = complex<double>(0, sqrt(1./2.));
        }
        else if (name == "-i"){
            this->vec[0] = sqrt(1./2.);
            this->vec[1] = complex<double>(0, -sqrt(1./2.));
        }
        else if (name == "0"){
            this->vec[0] = 1;
            this->vec[1] = 0;
        }
        else if (name == "1"){
            this->vec[0] = 0;
            this->vec[1] = 1;
        }
    }
    State::State(const State& state) : n(state.n), dim(state.dim) {
        vec = new std::complex<double>[dim];
        for (int i = 0; i < dim; ++i) {
            vec[i] = state.vec[i];
        }
    }
    
    State& State::operator=(const State& state) {
        if (this != &state) {
            delete[] vec;
            n = state.n;
            dim = state.dim;
            vec = new std::complex<double>[dim];
            for (int i = 0; i < dim; ++i) {
                vec[i] = state.vec[i];
            }
        }
        return *this;
    }
    State::~State(){
        delete[] vec;
    }
    bool State::operator==(const State& state){
        double threshold = 1e-8;
        for (int i = 0; i < dim; ++i){
            if (abs(this->vec[i] - state.vec[i]) > threshold){
                return false;
            }
        }
        return true;
    }
    bool State::operator!=(const State& state){
        return !((*this) == state);
    }
    ostream& operator<<(ostream& os, const State& state){
        os << '[';
        for (int i = 0; i < state.dim; ++i){
            os << state.vec[i].real();
            if (state.vec[i].imag() >= 0){
                os << '+';
            }
            os << state.vec[i].imag() << 'i';
            if (i < state.dim){
                os << ',';
            }
        }
        os << ']' << endl;
        return os;
    }
    State operator*(complex<double> c, const State& state){
        complex<double> v[state.dim];
        State s(v, state.n);
        for (int i = 0; i < state.dim; ++i){
            for (int j = 0; j < state.dim; ++j){
                v[i] = c * state.vec[i];
            }
        }
        return s;
    }
        
    Gate::Gate(complex<double>** mat, int n): n(n), dim(1 << n){
        this->mat = new complex<double>*[dim];
        for (int i = 0; i < dim; ++i){
            this->mat[i] = new complex<double>[dim];
        }
        for (int i = 0; i < dim; ++i){
            for (int j = 0; j < dim; ++j){
                this->mat[i][j] = mat[i][j];
            }
        }
    }
    Gate::Gate(const string& name){
        vector<string> unary_names{"i", "x", "y", "z", "s", "t", "h"};
        vector<string> binary_names{"cx", "cy", "cz", "ch"};
        vector<string> ternary_names{"ccx"};
        if (find(unary_names.begin(), unary_names.end(), name) != unary_names.end()){
            this->n = 1;
        }
        else if (find(binary_names.begin(), binary_names.end(), name) != binary_names.end()){
            this->n = 2;
        }
        else if (find(ternary_names.begin(), ternary_names.end(), name) != ternary_names.end()){
            n = 3;
        }
        else{
            throw runtime_error("Gate(const string& name): Invalid gate name");
        }
        this->dim = 1 << n;
        this->mat = new complex<double>*[dim];
        for (int i = 0; i < this->dim; ++i){
            this->mat[i] = new complex<double>[dim];
            for (int j = 0; j < this->dim; ++j){
                this->mat[i][j] = 0;
            }
        }
        if (name == "i"){
            this->mat[0][0] = 1, this->mat[1][1] = 1;
        }
        else if (name == "x"){
            this->mat[0][1] = 1, this->mat[1][0] = 1;
        }
        else if (name == "y"){
            this->mat[0][1] = complex<double>(0, -1);
            this->mat[1][0] = complex<double>(0, 1);
        }
        else if (name == "z"){
            this->mat[0][0] = 1, this->mat[1][1] = -1;
        }
        else if (name == "s"){
            this->mat[0][0] = 1, this->mat[1][1] = complex<double>(0, 1);
        }
        else if (name == "t"){
            this->mat[0][0] = 1;
            this->mat[1][1] = complex<double>(sqrt(1./2.), sqrt(1./2.));
        }
        else if (name == "h"){
            this->mat[0][0] = sqrt(1./2.), this->mat[0][1] = sqrt(1./2.);
            this->mat[1][0] = sqrt(1./2.), this->mat[1][1] = -sqrt(1./2.);
        }
        else if (name == "cx"){
            this->mat[0][0] = 1, this->mat[1][1] = 1;
            this->mat[2][3] = 1, this->mat[3][2] = 1;
        }
        else if (name == "cy"){
            this->mat[0][0] = 1, this->mat[1][1] = 1;
            this->mat[2][3] = complex<double>(0, -1), this->mat[3][2] = complex<double>(0, 1);
        }
        else if (name == "cz"){
            this->mat[0][0] = 1, this->mat[1][1] = 1;
            this->mat[2][2] = 1, this->mat[3][3] = -1;
        }
        else if (name == "ch"){
            this->mat[0][0] = 1, this->mat[1][1] = 1;
            this->mat[2][2] = sqrt(1./2.), this->mat[2][3] = sqrt(1./2.);
            this->mat[3][2] = sqrt(1./2.), this->mat[3][3] = -sqrt(1./2.);
        }
        else if (name == "ccx"){
            this->mat[0][0] = 1, this->mat[1][1] = 1;
            this->mat[2][2] = 1, this->mat[3][7] = 1;
            this->mat[4][4] = 1, this->mat[5][5] = 1;
            this->mat[6][6] = 1, this->mat[7][3] = 1;
        }
    }
    Gate::Gate(const Gate& other) : n(other.n), dim(other.dim) {
        mat = new std::complex<double>*[dim];
        for (int i = 0; i < dim; ++i) {
            mat[i] = new std::complex<double>[dim];
            for (int j = 0; j < dim; ++j) {
                mat[i][j] = other.mat[i][j];
            }
        }
    }
    Gate& Gate::operator=(const Gate& gate) {
        if (this != &gate) {
            for (int i = 0; i < dim; ++i)
                delete[] mat[i];
            delete[] mat;
            n = gate.n;
            dim = gate.dim;
            mat = new std::complex<double>*[dim];
            for (int i = 0; i < dim; ++i) {
                mat[i] = new std::complex<double>[dim];
                for (int j = 0; j < dim; ++j) {
                    mat[i][j] = gate.mat[i][j];
                }
            }
        }
        return *this;
    }
    Gate::~Gate(){
        for (int i = 0; i < dim; ++i){
            delete[] mat[i];
        }
        delete[] mat;
    }
    bool Gate::operator==(const Gate& gate){
        double threshold = 1e-8;
        for (int i = 0; i < dim; ++i){
            for (int j = 0; j < dim; ++j){
                if (abs(this->mat[i][j] - gate.mat[i][j]) > threshold){
                    return false;
                }
            }
        }
        return true;
    }
    bool Gate::operator!=(const Gate& gate){
        return !((*this) == gate);
    }
    Gate Gate::operator*(const Gate& gate){
        complex<double>** m = new complex<double>*[dim];
        for (int i = 0; i < dim; ++i){
            m[i] = new complex<double>[dim];
        }
        Gate g(m, gate.n);
        for (int i = 0; i < dim; ++i){
            for (int j = 0; j < dim; ++j){
                for (int k = 0; k < dim; ++k){
                    g.mat[i][j] += this->mat[i][k] * gate.mat[k][j];
                }
            }
        }
        return g;
    }
    State Gate::operator*(const State& state){
        complex<double>* v = new complex<double>[dim];
        State s(v, state.n);
        for (int j = 0; j < dim; ++j){
            for (int i = 0; i < dim; ++i){
                s.vec[j] += this->mat[i][j] * state.vec[i];
            }
        }
        complex<double> phase(0, 0);
        for (int i = 0; i < dim; ++i){
            if (abs(s.vec[i]) != 0){
                phase = s.vec[i] / abs(s.vec[i]);
                break;
            }
        }
        if (phase != complex<double>(0, 0) && phase != complex<double>(1, 0)){
            for (int i = 0; i < dim; ++i){
                s.vec[i] /= phase;
            }
        }
        return s;
    }
    Gate operator*(complex<double> c, const Gate& gate){
        complex<double>** m = new complex<double>*[gate.dim];
        for (int i = 0; i < gate.dim; ++i){
            m[i] = new complex<double>[gate.dim];
        }
        Gate g(m, gate.n);
        for (int i = 0; i < gate.dim; ++i){
            for (int j = 0; j < gate.dim; ++j){
                m[i][j] = c * gate.mat[i][j];
            }
        }
        return g;
    }
    ostream& operator<<(ostream& os, const Gate& gate){
        os << '[';
        for (int i = 0; i < gate.dim; ++i){
            os << '[';
            for (int j = 0; j < gate.dim; ++j){
                os << gate.mat[i][j].real();
                if (gate.mat[i][j].imag() >= 0){
                    os << '+';
                }
                os << gate.mat[i][j].imag() << 'i';
                if (j < gate.dim - 1){
                    os << ',';
                }
            }
            os << ']';
            if (i < gate.dim - 1){
                os << ',' << endl;;
            }
        }
        os << ']' << endl;
        return os;
    }
}


