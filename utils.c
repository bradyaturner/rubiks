#include "utils.h"

// TODO is this necessary? is passing size required?
int indexOf(const int arr[], const int size, const int element) {
	int index = -1;
	for (int i=0; i<size; i++) {
		if (arr[i] == element) {
			index = i;
			break;
		}
	}
	return index;
}

