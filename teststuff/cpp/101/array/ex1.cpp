
// C++ program to find the min and max element
// of Array using sort() in STL

#include <bits/stdc++.h>
#include <math.h>
using namespace std;

/// @brief Find index to closest value in arr
int getValIdx(int arr[6], int n, int pick=74) {
    int minArr[n];

    cout << endl;
    for(int i=0; i<n; i++) minArr[i] = abs(arr[i]-pick);

    int minVar[2] = {0, minArr[0]};
    cout << "picked: " << pick << endl;
    printf("[%d] %3d|%3d - %3d\n", 0, minVar[1], minArr[0], arr[0]);
    for(int i=1; i<n; i++) {
        printf("[%d] %3d|%3d - %3d\n", i, minVar[1], minArr[i], arr[i]);
        if(minVar[1]>minArr[i]) {
            minVar[0]=i;
            minVar[1]=minArr[i];
        }
    }
    printf("\nClosest value to %d: index:%d element:%d\n", pick, minVar[0], arr[minVar[0]]);

    return minVar[0];
}

int main()
{
	// Get the array
	int arr[] = { 1, 45, 54, 71, 76, 12 };

	int n = sizeof(arr) / sizeof(arr[0]);

	for (int i = 0; i < n; i++) cout << arr[i] << " ";

    int pick = 74;
    getValIdx(arr, n, pick);
    
    // int minArr[sizeof(arr)/sizeof(arr[0])];

    // for(int i=0; i<n; i++) { minArr[i] = abs(arr[i]-pick); }
    

    // int minVar[2] = {0, minArr[0]};
    // cout << "0|" <<  minVar[1] << "|" << minArr[0] << " ";
    // for(int i=1; i<n; i++) {
    //     cout << i << "|" << minVar[1] << "|" << minArr[i] << " ";
    //     if(minVar[1]>minArr[i]) {
    //         minVar[0]=i;
    //         minVar[1]=minArr[i];
    //     }
    // }
    // printf("\nClosest value to %d: index:%d element:%d\n", pick, minVar[0], arr[minVar[0]]);

	// // Find the minimum element
	// cout << "\nMin Element = "
	// 	<< *min_element(arr, arr + n);

	// // Find the maximum element
	// cout << "\nMax Element = "
	// 	<< *max_element(arr, arr + n);
	// // Storing the pointer in an address
	// int &min = *min_element(arr,arr+n );
	// int &max = *max_element(arr,arr+n );
	// cout<<"\nFinding the Element using address variable";
	// cout<<"\nMin Element = "<<min;
	// cout<<"\nMax Element = "<<max;

    // cout << endl;
	return 0;
}
