#ifndef __MATRIXNN_H__
#define __MATRIXNN_H__
#include "thread_helpers.hpp"

#include <cstdio>
#include <cstring>
#include <random>
#include <vector>

class MatrixNN{
public:
  MatrixNN(const int &n){
		n_ = n;
		data_.resize(n_*n_);
		std::fill(data_.begin(),data_.end(),0.0);
	}
	
	MatrixNN(const MatrixNN &m){
		n_ = m.n_;
		std::copy(m.data_.begin(), m.data_.end(),data_.begin());
	}
	
	void Identity() {
		for(int r = 0; r < n_; r++){
			for(int c = 0; c < n_; c++){
				if(r==c)(*this)(r,c) = 1.0;
			}
		}	
	}

	void FillRandom(){
		std::default_random_engine generator;
		std::uniform_real_distribution<double> distribution(1.0,2.0);
		for(int i = 0; i < data_.size(); i++){
			data_[i] = distribution(generator);		
		}
	}
	

	int get_size()const{return data_.size();}
	

	int get_n()const{return n_;}

	MatrixNN QuickMult(const MatrixNN& m, const int nbThreads)const{
		MatrixNN tmp(m.get_n());
		const MatrixNN *a = this;
		auto myFunc = [&](unsigned int i){
			int r = i/tmp.get_n();
			int c = i-(r*tmp.get_n());
			for(int rm = 0; rm < n_; rm++){
				tmp(r,c) += (*a)(r,rm) * m(rm,c);
			}
		};
		parallelFor(data_.size(),nbThreads,myFunc);
		return tmp;
	}
	
	MatrixNN SlowMult(const MatrixNN& m)const{
		MatrixNN tmp(m.get_n());
		const MatrixNN *a = this;
		for(int r = 0; r < n_; r++){
			for(int c = 0; c < n_; c++){
				for(int rm = 0; rm < n_; rm++){
					tmp(r,c) += (*this)(r,rm) * m(rm,c); 
				}
			}
		}
		return tmp;
	}
	
	MatrixNN operator*(const MatrixNN& m)const{
		MatrixNN tmp(m.get_n());
		const MatrixNN *a = this;
		for(int r = 0; r < n_; r++){
			for(int c = 0; c < n_; c++){
				for(int rm = 0; rm < n_; rm++){
					tmp(r,c) += (*this)(r,rm) * m(rm,c); 
				}
			}
		}
		return tmp;
	}
	
	bool operator==(const MatrixNN &rhs)const{
		if(data_.size() != rhs.get_size())return false;
		if(n_ != rhs.get_n())return false;
	
		for(int i = 0; i < data_.size(); i++){
			if(data_[i] != rhs.data_[i])return false;
		}	
		return true;
	}
	
	void operator=(const MatrixNN &m){
		n_ = m.n_;
		data_.resize(n_*n_);
		std::copy(m.data_.begin(), m.data_.end(),data_.begin());
	}
	
	double operator()(const int& row, const int& col)const{
		int index = row * n_ + col;
		return data_[index]; 
	}
	
	double &operator()(const int& row, const int& col){
		int index = row * n_ + col;
		return data_[index]; 
	}
	
	double &operator[](const int& index){
		return data_[index];
	}
private:
	std::vector<double> data_;
	int n_;
};

#endif //__MATRIXNN_H__
