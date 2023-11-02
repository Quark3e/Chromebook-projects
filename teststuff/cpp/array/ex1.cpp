
// C++ program to find the min and max element
// of Array using sort() in STL

#include <bits/stdc++.h>
#include <math.h>
using namespace std;

/// @brief Find index to closest value in arr
int findVal(int arr, int n, int pick=74) {
    int minArr[sizeof(arr)/sizeof(arr[0])];

    for(int i=0; i<n; i++) { minArr[i] = abs(arr[i]-pick); }
    

    int minVar[2] = {0, minArr[0]};
    cout << "0|" <<  minVar[1] << "|" << minArr[0] << " ";
    for(int i=1; i<n; i++) {
        cout << i << "|" << minVar[1] << "|" << minArr[i] << " ";
        if(minVar[1]>minArr[i]) {
            minVar[0]=i;
            minVar[1]=minArr[i];
        }
    }
    printf("\nClosest value to %d: \n", pick);
    printf("index:%d \n", minVar[0]);
    // cout << sizeof(arr)/sizeof(arr[0]) << endl;
    // printf("element:%d", arr);

    // return minVar[0];
    cout << "test" << endl;
    return 0;
}

int main()
{
	// Get the array
	int arr[] = { 1, 45, 54, 71, 76, 12 };


	// Compute the sizes
	int n = sizeof(arr) / sizeof(arr[0]);

	// Print the array
	cout << "Array: ";
	for (int i = 0; i < n; i++)
		cout << arr[i] << " ";
    cout << "\nLength of array: " << n << endl;

    int pick = 74;
    findVal(arr, n, pick);
    
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
