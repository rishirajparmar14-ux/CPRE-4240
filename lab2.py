import random
import time

numbers = [3, 1, 4, 1, 5, 9, 2, 6]
print("Starting list:", numbers)
print()

# append(x) 
numbers.append(10)
print("After append(10):", numbers)
print()

# insert(index, x)
numbers.insert(0, 100)
print("After insert(0, 100):", numbers)
print()

# extend(iterable) 
numbers.extend([7, 8])
print("After extend([7, 8]):", numbers)
print()

# count(x) 
how_many_ones = numbers.count(1)
print("Number of 1's in the list:", how_many_ones)
print()

# index(x)
where_is_9 = numbers.index(9)
print("Index of first 9:", where_is_9)
print()

# copy() 
numbers_copy = numbers.copy()
numbers_copy.append(999)
print("Original list :", numbers)
print("Copied list   :", numbers_copy)
print()

# sort() 
numbers.sort()
print("After sort():", numbers)
print()

# reverse() 
numbers.reverse()
print("After reverse():", numbers)
print()

# remove(x) 
numbers.remove(100)
print("After remove(100):", numbers)
print()

# pop() 
last_item = numbers.pop()
print("pop() removed:", last_item)
print("List after pop():", numbers)
print()

# pop(index) 
first_item = numbers.pop(0)
print("pop(0) removed:", first_item)
print("List after pop(0):", numbers)
print()

# clear() 
numbers.clear()
print("After clear():", numbers)

N = 5000

for N in [N, 2 * N, 4 * N, 8 * N]:

    # cost of L.pop() (remove from end)
    L = [random.random() for _ in range(N)]
    start = time.perf_counter()
    L.pop()
    end = time.perf_counter()
    print("N =", N, "pop() time:", end - start)

    # cost of L.pop(0) (remove from front)
    L = [random.random() for _ in range(N)]
    start = time.perf_counter()
    L.pop(0)
    end = time.perf_counter()
    print("N =", N, "pop(0) time:", end - start)

    # in-place modification: L.reverse()
    L = [random.random() for _ in range(N)]
    start = time.perf_counter()
    L.reverse()
    end = time.perf_counter()
    print("N =", N, "reverse() time:", end - start)

    # out-of-place: R = L[::-1]
    L = [random.random() for _ in range(N)]
    start = time.perf_counter()
    R = L[::-1]
    end = time.perf_counter()
    print("N =", N, "R = L[::-1] time:", end - start)

    print()
