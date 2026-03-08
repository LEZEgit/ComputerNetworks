# ComputerNetworks Self Assignments

## Single Bit Error Correction (SEC)

This project implements a simplified **Error Correcting Code (ECC)** based on parity groups. It simulates the process of encoding a data frame, introducing a random bit-flip error, and using set theory to isolate and correct the corrupted bit.

### 🛠 How it Works

1. **Parity Generation:** The data frame is divided into overlapping groups ($G_0, G_1, G_2$). A parity bit is generated for each group and appended to the data to form a **Codeword**.
2. **Error Simulation:** A single bit (either in the data or the parity section) is randomly flipped to simulate transmission noise.
3. **Syndrome Detection:** The receiver recalculates parity for all groups. If the calculated parity doesn't match the received parity, that group is added to a `failParity` set.
4. **Bit Isolation:** If only one parity bit fails, the error is in the parity bit itself.
* If multiple groups fail, the error is in a data bit. The code finds the **intersection** of all failed groups and **subtracts** any bits belonging to groups that passed. The remaining bit is the culprit.



---

### ⚠️ Lessons Learned & Bug Fixes

#### 1. Iterator Invalidation (The Segmentation Fault)

During development, a `std::set_intersection` approach was used to isolate the erroneous bit. A segmentation fault occurred in the following logic:

```cpp
// ❌ WRONG: Causes Segmentation Fault
for(auto ele: commonBits){
  if(Gs[i].count(ele) != 0) 
    commonBits.erase(ele); // Error: Modifying the set while iterating!
}

```

**The Issue:** In C++, erasing an element from a container while iterating over it invalidates the iterator. When the loop tries to move to the "next" element, it points to a memory location that no longer exists or has shifted.

**The Fix:** Store the elements intended for deletion in a temporary `std::vector` (a "trash" list), then iterate through that vector to erase them from the set after the main loop finishes.

#### 2. The Ghost of `cout` (Buffering vs. Flushing)

Earlier this was in the code
```
sec.cpp

else {
    cout<<"\nfailParitySet size= "<< failParity.size()<<endl;
    // find the common bits in failed parity set
    unordered_set<int> commonBits={};
    for(auto in: failParity){
      cout<<"Fail parity: "<<in<<endl;
      if(commonBits.empty()) 
        commonBits = Gs[in];
      else 
        commonBits = set_intrscn(commonBits, Gs[in]);
    }
    cout<<"\nCommon Bits: (1-indexed) ";
    for(auto ele: commonBits){
      cout<<ele<<" ";
    }

 // remove the bits that are in groups that have correct parity (not in failParity)
    for(int i=0; i<k; i++){ // k = number of groups
      if(failParity.count(i)==0){ // Gs[i] has correct parity
        for(auto ele: commonBits){
          if(Gs[i].count(ele)!=0) commonBits.erase(ele); // <-- 
        }

```

Earlier when the program was giving sementation fault, the terminal looked like
```
SENDER! Enter the dataframe: 1011
Inside generateParity
Inside generateParity
Inside generateParity
in the constructCodeword codew = 1011100
Error created at 0
After transmission, receiver received frame: 0011100
Error detected!
Working on correction:

Inside sec
Inside generateParity
Inside generateParity
Inside generateParity
fail parity set size= 2
Fail parity: 1
Fail parity: 0

Inside set_intrscn
[1]    29864 segmentation fault (core dumped)  ./secc
```
which made it seem like segfault was caused due to the function set_intrscn.

But upon researching I found that the issue was in the later for loop.
So that confused me, if the fault is caused due to a loop after the 
`cout<<"Common Bits"` then why am I not seeing that cout. Upon researching a bit, here is what I found:

* **Buffering:** `std::cout` is buffered for performance. It collects characters in memory and only prints them when the buffer is full or a "flush" is triggered.
* **The Difference:** `\n` merely adds a newline, while `std::endl` adds a newline **and** flushes the buffer.
* **Why it mattered:** Because the program crashed (SegFault), the memory buffer was wiped before the OS could print the "Common Bits" string. Using `std::endl` or `std::flush` during debugging ensures the terminal is up-to-date with the code's actual execution point.




### Final Running:
```
╭─ ~/CN_ass/singleBitErrorCorrection main ·········
╰─❯ g++ sec.cpp -o sec

╭─ ~/CN_ass/singleBitErrorCorrection main ·········
╰─❯ ./sec

----------------------
SENDER! Enter the dataframe: 1011
in the constructCodeword codew = 1011100
Error created at 4
After transmission, receiver received frame: 1011000
Error detected!
Working on correction: Error in the Parity bit
Error at 4 fixed!

----------------------
SENDER! Enter the dataframe: 1011
in the constructCodeword codew = 1011100
Error created at 4
After transmission, receiver received frame: 1011000
Error detected!
Working on correction: Error in the Parity bit
Error at 4 fixed!

----------------------
SENDER! Enter the dataframe: 1011
in the constructCodeword codew = 1011100
Error created at 6
After transmission, receiver received frame: 1011101
Error detected!
Working on correction: Error in the Parity bit
Error at 6 fixed!

----------------------
SENDER! Enter the dataframe: 1011
in the constructCodeword codew = 1011100
Error created at 2
After transmission, receiver received frame: 1001100
Error detected!
Working on correction:
failParitySet size= 3
Fail parity: 2
Fail parity: 1
Fail parity: 0

Common Bits: (1-indexed) 3
Error in the data word
Error at 2 fixed!

----------------------
SENDER! Enter the dataframe: X
Closing the program!
-------------
```

