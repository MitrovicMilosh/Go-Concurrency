#include <iostream> 
#include <vector> 
#include <algorithm> 
#include <ctime> 
#include <future> 
#include <cstdlib> 
  
void quick_sort(std::vector<int>::iterator first, std::vector<int>::iterator last) 
{ 
    if(last-first>1) 
    { 
        //find pivot with median of three 
        std::vector<int>::iterator mid(first + (last-first)/2); 
        if(*first>*mid) std::swap(*first, *mid); 
        if(*first>*(last-1)) std::swap(*first, *(last-1)); 
        if(*(last-1)>*mid) std::swap(*mid, *(last-1)); 
  
        //in-place partition 
        std::vector<int>::iterator i(first-1), j(last-1); 
        for(;;) 
        { 
            while(*(++i) < *(last-1) ); 
            while(*(--j) > *(last-1) ); 
            if(i>=j) break; 
            std::swap(*i, *j); 
        } 
        std::swap(*i, *(last-1)); 
  
        quick_sort(first, i); 
        quick_sort(i+1, last); 
    } 
} 
  
void quick_sort_parallel(std::vector<int>::iterator first, 
                          std::vector<int>::iterator last, 
                          unsigned remaining_threads) 
{ 
    if(last-first>1) 
    { 
        //find pivot with median of three 
        std::vector<int>::iterator mid(first + (last-first)/2); 
        if(*first>*mid) std::swap(*first, *mid); 
        if(*first>*(last-1)) std::swap(*first, *(last-1)); 
        if(*(last-1)>*mid) std::swap(*mid, *(last-1)); 
  
        //in-place partition 
        std::vector<int>::iterator i(first-1), j(last-1); 
        for(;;) 
        { 
            while(*(++i) < *(last-1) ); 
            while(*(--j) > *(last-1) ); 
            if(i>=j) break; 
            std::swap(*i, *j); 
        } 
        std::swap(*i, *(last-1)); 
  
        //recursion 
        if(remaining_threads) 
        { 
            std::future<void> thread1 = std::async(quick_sort_parallel, first, i, remaining_threads-1);
			std::future<void> thread2 = std::async(quick_sort_parallel, i+1, last, remaining_threads-1);  
            
            thread1.wait();
			thread2.wait(); 
        } 
        else 
        { 
            quick_sort(first, i); 
            quick_sort(i+1, last); 
        } 
    } 
} 
  
int main(int argc, char* argv[]) 
{ 
	int n = 0;
	if(argc > 1)
		n = atoi(argv[1]);
	
    int size = 1000000; 
    std::vector<int> test(size); 
    std::generate(test.begin(), test.end(), [](){ return std::rand(); }); 
  
    quick_sort_parallel(test.begin(), test.end(), n); 
}
