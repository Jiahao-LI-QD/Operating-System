#include <stdio.h>
#include <stdlib.h>

int mask_page = 0xff00;
int mask_off = 0x00ff;

int table[256][2];

int tlb[16][2];
int tlbp, tlbc;

void mmu_256(FILE* in, FILE* out);
void mmu_128(FILE* in, FILE* out);

int input, tlbhit, pagefault;

int main(int argc, char** argv){
    FILE *bin, *in;
    bin = fopen(argv[2], "r");
    in = fopen(argv[3], "r");

    if (atoi(argv[1]) == 256){
        mmu_256(bin, in);
    }
    else if (atoi(argv[1]) == 128) {
        mmu_128(bin, in);
    }
    else {
        printf("Error: wrong memory size!\n");
    }
    fclose(bin);
    fclose(in);
    return 0;
}

void mmu_256(FILE* bin, FILE* in){
    FILE *out = fopen("output256.csv", "w");
    char str[10];
    char physical[256][256];
    int pc;

    int page_n, offset;
    while (fgets(str, 10, in)){
        int i, frame;
        short c;
        page_n = (atoi(str) & mask_page) >> 8;
        offset = atoi(str) & mask_off;
        input ++;
		
		for (i = 0; i < tlbc; i ++){
		 	if (tlb[i][0] == page_n){
		 		frame = tlb[i][1];
		 		tlbhit ++;
		 		break;
		 	} 
		}
		if (i == tlbc){ // tlb miss
			if (table[page_n][1]==0){ //page fault
            	pagefault ++;
            	table[page_n][1] = 1;
            	table[page_n][0] = pc++;
        	    fseek(bin,page_n * 256, SEEK_SET);
            	fread(physical[table[page_n][0]], 256, 1, bin);
        	}
        	
        	if (tlbc<16){ // tlb not full stack
        	 	tlb[tlbc][0] = page_n;
        	 	tlb[tlbc][1] = table[page_n][0];
                tlbc++;
			}
			else {
			 	tlb[tlbp][0] = page_n;
			 	tlb[tlbp][1] = table[page_n][0];
                if(tlbp++ == 15){
			 		tlbp = 0;	
				} 
			}
		}
        
        frame = table[page_n][0];
        c = physical[frame][offset];
        fprintf(out, "%d,%d,%d\n", atoi(str), (frame * 256 + offset), c);

    }
    
    fprintf(out, "Page Faults Rate, %.2f%%,\n", ((float) pagefault / input) * 100);
    fprintf(out, "TLB Hits Rate, %.2f%%,", ((float) tlbhit / input) * 100);
    
    fclose(out);
    
}


void mmu_128(FILE* bin, FILE* in){
    FILE *out = fopen("output128.csv", "w");
    char str[10];

    char physical[128][256];
    int count, record[128][2];

    int page_n, offset;
    while (fgets(str, 10, in)){
        int i, frame;
        short c;
        page_n = (atoi(str) & mask_page) >> 8;
        offset = atoi(str) & mask_off;
        input ++;
		
		for (i = 0; i < tlbc; i ++){
			if (tlb[i][0] == page_n){
				frame = tlb[i][1];
				record[frame][0] = input;
                tlbhit++;
				break;
			} 
		}
		
		if (i == tlbc){ // tlb miss
		
			if (table[page_n][1]==0){ //page fault
            	pagefault ++;
            	if (count < 128){
            		table[page_n][1] = 1;
            		table[page_n][0] = count;
            		record[count][0] = input;
            		record[count++][1] = page_n;
				}
				else {
					int  j = 0, min;
					min = input;
					while (j < 128){
						if (record[j][0] < min){
							min = record[j][0];
							i = j;
						}
						j++;
					}
					table[record[i][1]][1]= 0;
					//table[record[i][1]][0]= 0;
					table[page_n][1] = 1;
					table[page_n][0] = i; 
					record[i][0] = input;
					record[i][1] = page_n;
				}
        	    fseek(bin,page_n * 256, SEEK_SET);
            	fread(physical[table[page_n][0]], 256, 1, bin);
        	}
        	else {
        		record[table[page_n][0]][0] = input;
			}
        	
        	if (tlbc<16){ // tlb not full stack
        	 	tlb[tlbc][0] = page_n;
        	 	tlb[tlbc][1] = table[page_n][0];
                tlbc++;
			}
			else {
			 	tlb[tlbp][0] = page_n;
			 	tlb[tlbp][1] = table[page_n][0];
                if(tlbp++ == 15){
			 		tlbp = 0;	
				} 
			}
            frame = table[page_n][0];
		}
        c = physical[frame][offset];
        fprintf(out, "%d,%d,%d\n", atoi(str), (frame * 256 + offset), c);

    }

    fprintf(out, "Page Faults Rate, %.2f%%,\n", ((float) pagefault / input) * 100);
    fprintf(out, "TLB Hits Rate, %.2f%%,", ((float) tlbhit / input) * 100);

    fclose(out);
}