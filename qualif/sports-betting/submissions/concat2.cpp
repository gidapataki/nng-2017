
#include<stdlib.h>
#include<cfloat>
#include<cmath>
#include<limits>
#include<iostream>
#pragma once
#ifndef NUMBER_HPP_
#define NUMBER_HPP_
#include<boost/rational.hpp>
using Number=boost::rational<long long>;
#endif
#include<vector>
Number solveHungarian(std::vector<std::vector<Number>>&,std::vector<int>&);using namespace std;void f4(int*,Number*,bool*,bool*,bool*,bool*,bool*,int,int,int);void f5(int*,Number*,bool*,bool*,bool*,bool*,bool*,int,int,int);void f0(int*,Number*,bool*,bool*,bool*,bool*,bool*,int,int,int);void f1(int*,Number*,bool*,bool*,bool*,bool*,bool*,int,int,int,int,int);void f2(int*,Number*,bool*,bool*,bool*,bool*,bool*,int,int,int);void f6(int*,Number*,Number*,int);void f7(int*v29,Number*v13,Number*v30,int v10,int v24){Number*v28,*v0,*v21,*v6,v8,v14;bool*v17,*v4,*v26,*v7,*v27;int v25,v12,v18,v15;*v13=Number{0};for(v18=0;v18<v10;v18++)v29[v18]=-1;v25=v10*v24;v28=new Number[v25];v21=v28+v25;for(v18=0;v18<v25;v18++){v8=v30[v18];if(v8<Number{0})cerr<<"ERROR"<<endl;v28[v18]=v8;}v17=(bool*)calloc(v24,sizeof(bool));v4=(bool*)calloc(v10,sizeof(bool));v26=(bool*)calloc(v25,sizeof(bool));v27=(bool*)calloc(v25,sizeof(bool));v7=(bool*)calloc(v25,sizeof(bool));if(v10<=v24){v12=v10;for(v18=0;v18<v10;v18++){v0=v28+v18;v14=*v0;v0+=v10;while(v0<v21){v8=*v0;if(v8<v14)v14=v8;v0+=v10;}v0=v28+v18;while(v0<v21){*v0-=v14;v0+=v10;}}for(v18=0;v18<v10;v18++)for(v15=0;v15<v24;v15++)if(fabs(boost::rational_cast<double>(v28[v18+v10*v15]))<DBL_EPSILON)if(!v17[v15]){v26[v18+v10*v15]=true;v17[v15]=true;break;}}else{v12=v24;for(v15=0;v15<v24;v15++){v0=v28+v10*v15;v6=v0+v10;v14=*v0++;while(v0<v6){v8=*v0++;if(v8<v14)v14=v8;}v0=v28+v10*v15;while(v0<v6)*v0++-=v14;}for(v15=0;v15<v24;v15++)for(v18=0;v18<v10;v18++)if(fabs(boost::rational_cast<double>(v28[v18+v10*v15]))<DBL_EPSILON)if(!v4[v18]){v26[v18+v10*v15]=true;v17[v15]=true;v4[v18]=true;break;}for(v18=0;v18<v10;v18++)v4[v18]=false;}f5(v29,v28,v26,v7,v27,v17,v4,v10,v24,v12);f6(v29,v13,v30,v10);delete[] v28;free(v17);free(v4);free(v26);free(v27);free(v7);return;}void f3(int*v29,bool*v26,int v10,int v24){int v18,v15;for(v18=0;v18<v10;v18++)for(v15=0;v15<v24;v15++)if(v26[v18+v10*v15]){
#ifdef ONE_INDEXING
v29[v18]=v15+1;
#else 
v29[v18]=v15;
#endif
break;}}void f6(int*v29,Number*v13,Number*v28,int v10){int v18,v15;for(v18=0;v18<v10;v18++){v15=v29[v18];if(v15>=0)*v13+=v28[v18+v10*v15];}}void f4(int*v29,Number*v28,bool*v26,bool*v7,bool*v27,bool*v17,bool*v4,int v10,int v24,int v12){bool*v5,*v6;int v15;for(v15=0;v15<v24;v15++){v5=v26+v10*v15;v6=v5+v10;while(v5<v6){if(*v5++){v17[v15]=true;break;}}}f5(v29,v28,v26,v7,v27,v17,v4,v10,v24,v12);}void f5(int*v29,Number*v28,bool*v26,bool*v7,bool*v27,bool*v17,bool*v4,int v10,int v24,int v12){int v15,v23;v23=0;for(v15=0;v15<v24;v15++)if(v17[v15])v23++;if(v23==v12){f3(v29,v26,v10,v24);}else{f0(v29,v28,v26,v7,v27,v17,v4,v10,v24,v12);}}void f0(int*v29,Number*v28,bool*v26,bool*v7,bool*v27,bool*v17,bool*v4,int v10,int v24,int v12){bool v16;int v18,v15,v20;v16=true;while(v16){v16=false;for(v15=0;v15<v24;v15++)if(!v17[v15])for(v18=0;v18<v10;v18++)if((!v4[v18])&&(fabs(boost::rational_cast<double>(v28[v18+v10*v15]))<DBL_EPSILON)){v27[v18+v10*v15]=true;for(v20=0;v20<v24;v20++)if(v26[v18+v10*v20])break;if(v20==v24){f1(v29,v28,v26,v7,v27,v17,v4,v10,v24,v12,v18,v15);return;}else{v4[v18]=true;v17[v20]=false;v16=true;break;}}}f2(v29,v28,v26,v7,v27,v17,v4,v10,v24,v12);}void f1(int*v29,Number*v28,bool*v26,bool*v7,bool*v27,bool*v17,bool*v4,int v10,int v24,int v12,int v18,int v15){int n,v19,v20,v9,v11;int v25=v10*v24;for(n=0;n<v25;n++)v7[n]=v26[n];v7[v18+v10*v15]=true;v20=v15;for(v19=0;v19<v10;v19++)if(v26[v19+v10*v20])break;while(v19<v10){v7[v19+v10*v20]=false;v9=v19;for(v11=0;v11<v24;v11++)if(v27[v9+v10*v11])break;v7[v9+v10*v11]=true;v20=v11;for(v19=0;v19<v10;v19++)if(v26[v19+v10*v20])break;}for(n=0;n<v25;n++){v27[n]=false;v26[n]=v7[n];}for(n=0;n<v10;n++)v4[n]=false;f4(v29,v28,v26,v7,v27,v17,v4,v10,v24,v12);}void f2(int*v29,Number*v28,bool*v26,bool*v7,bool*v27,bool*v17,bool*v4,int v10,int v24,int v12){Number h,v8;int v18,v15;h=Number{std::numeric_limits<int>::max()};for(v18=0;v18<v10;v18++)if(!v4[v18])for(v15=0;v15<v24;v15++)if(!v17[v15]){v8=v28[v18+v10*v15];if(v8<h)h=v8;}for(v18=0;v18<v10;v18++)if(v4[v18])for(v15=0;v15<v24;v15++)v28[v18+v10*v15]+=h;for(v15=0;v15<v24;v15++)if(!v17[v15])for(v18=0;v18<v10;v18++)v28[v18+v10*v15]-=h;f0(v29,v28,v26,v7,v27,v17,v4,v10,v24,v12);}Number solveHungarian(vector<vector<Number>>&v2,vector<int>&v1){unsigned int v22=v2.size();unsigned int v3=v2[0].size();Number*v30=new Number[v22*v3];int*v29=new int[v22];Number v13=Number{0};for(unsigned int i=0;i<v22;i++)for(unsigned int j=0;j<v3;j++)v30[i+v22*j]=v2[i][j];f7(v29,&v13,v30,v22,v3);v1.clear();for(unsigned int r=0;r<v22;r++)v1.push_back(v29[r]);delete[] v30;delete[] v29;return v13;}
#ifndef SOLVE_HPP_
#define SOLVE_HPP_
#ifndef NUMBER_HPP_
#define NUMBER_HPP_
#include<boost/rational.hpp>
using Number=boost::rational<long long>;
#endif
#include<vector>
Number solve(const std::vector<std::vector<Number>>&offers,const std::vector<std::size_t>&bookieLimits);
#endif
#pragma once
#ifndef NUMBER_HPP_
#define NUMBER_HPP_
#include<boost/rational.hpp>
using Number=boost::rational<long long>;
#endif
#include<vector>
Number solveHungarian(std::vector<std::vector<Number>>&,std::vector<int>&);
#include<boost/iterator/zip_iterator.hpp>
#include<boost/range/iterator_range.hpp>
#include<algorithm>
#include<numeric>
namespace{std::vector<std::vector<Number>>expandMatrix(const std::vector<std::vector<Number>>&offers,const std::vector<std::size_t>bookieLimits){std::vector<std::vector<Number>>result;result.reserve(offers.size());for(const auto&racerOffers: offers){auto begin=boost::make_zip_iterator(boost::make_tuple(racerOffers.begin(),bookieLimits.begin()));auto end=boost::make_zip_iterator(boost::make_tuple(racerOffers.end(),bookieLimits.end()));auto range=boost::make_iterator_range(begin,end);std::vector<Number>expandedRacerOffers;for(const auto&pair: range){Number odds=pair.get<0>();std::size_t multiplicity=pair.get<1>();for(std::size_t i=0;i<multiplicity;++i){expandedRacerOffers.push_back(odds);}}result.push_back(std::move(expandedRacerOffers));}return result;}std::size_t totalBetsAvailable(const std::vector<std::size_t>&bookieLimits){return std::accumulate(bookieLimits.begin(),bookieLimits.end(),0);}std::vector<std::vector<Number>>invert(const std::vector<std::vector<Number>>&m){std::vector<std::vector<Number>>result;result.reserve(m.size());for(const auto&row: m){std::vector<Number>invertedRow;invertedRow.reserve(row.size());for(const auto&n: row){if(n){invertedRow.push_back(1ll/n);}else{invertedRow.push_back(Number{100000});}}result.push_back(std::move(invertedRow));}return result;}}Number solve(const std::vector<std::vector<Number>>&offers,const std::vector<std::size_t>&bookieLimits){if(totalBetsAvailable(bookieLimits)<offers.size()){return Number{0};}const auto expandedMatrix=expandMatrix(offers,bookieLimits);auto invertedMatrix=invert(expandedMatrix);std::vector<int>assignment;auto naiveCost=solveHungarian(invertedMatrix,assignment);std::vector<Number>chosenOdds;chosenOdds.reserve(assignment.size());for(std::size_t i=0;i<assignment.size();++i){chosenOdds.push_back(expandedMatrix[i][assignment[i]]);}if(std::find(chosenOdds.begin(),chosenOdds.end(),Number{0})!=chosenOdds.end()){return Number{0};}if(naiveCost){return 1ll/naiveCost;}else{return Number{0};}}
#ifndef NUMBER_HPP_
#define NUMBER_HPP_
#include<boost/rational.hpp>
using Number=boost::rational<long long>;
#endif
#ifndef SOLVE_HPP_
#define SOLVE_HPP_
#ifndef NUMBER_HPP_
#define NUMBER_HPP_
#include<boost/rational.hpp>
using Number=boost::rational<long long>;
#endif
#include<vector>
Number solve(const std::vector<std::vector<Number>>&offers,const std::vector<std::size_t>&bookieLimits);
#endif
#include<iostream>
#include<vector>
std::istream&parseNumber(std::istream&is,Number&n){Number::int_type numerator;is>>numerator;if(is.good()&&is.peek()=='/'){is.get();Number::int_type denominator;is>>denominator;n=Number{numerator,denominator};}else{n=Number{numerator};}return is;}std::ostream&printNumber(std::ostream&os,const Number&n){if(n.denominator()==Number::int_type{1}){return os<<n.numerator();}else{return os<<n.numerator()<<'/'<<n.denominator();}}int main(){std::size_t numberOfTests=0;std::cin>>numberOfTests;for(std::size_t test=0;test<numberOfTests;++test){std::size_t numberOfRacers;std::size_t numberOfBookies;std::cin>>numberOfRacers>>numberOfBookies;std::vector<std::vector<Number>>racerOfferses;for(std::size_t racer=0;racer<numberOfRacers;++racer){std::vector<Number>racerOffers;racerOffers.reserve(numberOfBookies);for(std::size_t bookie=0;bookie<numberOfBookies;++bookie){Number offer;parseNumber(std::cin,offer);racerOffers.push_back(offer);}racerOfferses.push_back(std::move(racerOffers));}std::vector<std::size_t>bookieLimits;bookieLimits.reserve(numberOfBookies);for(std::size_t bookie=0;bookie<numberOfBookies;++bookie){std::size_t bookieLimit=0;std::cin>>bookieLimit;bookieLimits.push_back(bookieLimit);}printNumber(std::cout,solve(racerOfferses,bookieLimits));std::cout<<std::endl;}}
