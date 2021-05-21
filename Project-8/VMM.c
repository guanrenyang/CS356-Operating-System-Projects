# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define PAGE_NUMBER 256
# define PAGE_SIZE 256
# define FRAME_NUMBER 128
# define FRAME_SIZE 256
# define TLB_SIZE 16

struct empty_frame {
	int frame_number;
	struct empty_frame *next;
};

struct empty_frame *head = NULL;
struct empty_frame *tail = NULL;

int page_table[PAGE_NUMBER];
int valid[PAGE_NUMBER];
int page_fault_count;

char memory[FRAME_NUMBER * FRAME_SIZE];
int LRU[FRAME_NUMBER];
char buf[FRAME_SIZE];
FILE *backing_store;

int TLB_page[TLB_SIZE], TLB_frame[TLB_SIZE];
int TLB_LRU[TLB_SIZE];
int TLB_hit;

void add_frame(int frame_number) {
	if (head == NULL && tail == NULL) {
		tail = (struct empty_frame *) malloc (sizeof(struct empty_frame));
		tail -> frame_number = frame_number;
		tail -> next = NULL;
		head = tail;
	} else {
		tail -> next = (struct empty_frame *) malloc (sizeof(struct empty_frame));
		tail -> next -> frame_number = frame_number;
		tail -> next -> next = NULL;
		tail = tail -> next;
	}
}
int get_frame() {
	if (head == NULL && tail == NULL) return -1;
	int frame_number;
	if (head == tail) {
		frame_number = head -> frame_number;
		free(head);
		head = tail = NULL;
		return frame_number;
	}
	struct empty_frame *tmp;	
	frame_number = head -> frame_number;
	tmp = head;
	head = head -> next;
	free(tmp);
	return frame_number;
}
void initialize_frame() {
	for (int i = 0; i < FRAME_NUMBER; ++ i)
		add_frame(i);
}


void delete_TLB(int page_number, int frame_number);
void page_table_delete(int frame_number)
{
	int page_number = -1;
	for (int i = 0; i < PAGE_NUMBER; ++ i)
		if(valid[i] && page_table[i] == frame_number) {
			page_number = i;
			break;
		}
	valid[page_number] = 0;
	delete_TLB(page_number, frame_number);
}
int add_page(int page_number) {
	fseek(backing_store, page_number * FRAME_SIZE, SEEK_SET);
  fread(buf, sizeof(char), FRAME_SIZE, backing_store);
	
	int frame_number = get_frame();
	if (frame_number == -1) {
		for (int i = 0; i < FRAME_NUMBER; ++ i)
			if (LRU[i] == FRAME_NUMBER) {
				frame_number = i;
				break;
			}
		page_table_delete(frame_number);
	}

	for (int i = 0; i < FRAME_SIZE; ++ i)
		memory[frame_number * FRAME_SIZE + i] = buf[i];
	for (int i = 0; i < FRAME_NUMBER; ++ i)
		if (LRU[i] > 0) ++ LRU[i];
	LRU[frame_number] = 1;
	return frame_number;
}

char access_memory(int frame_number, int offset) {
	char res = memory[frame_number * FRAME_SIZE + offset];
	for (int i = 0; i < FRAME_NUMBER; ++ i)
		if (LRU[i] > 0 && LRU[i] < LRU[frame_number])
			++ LRU[i];
	LRU[frame_number] = 1;
	return res;
}

int get_TLB_frame_num(int page_number) {
	int pos = -1;
	for (int i = 0; i < TLB_SIZE; ++ i)
		if (TLB_LRU[i] > 0 && TLB_page[i] == page_number) {
			pos = i;
			break;
		}

	if (pos == -1) return -1;
	++ TLB_hit;
	for (int i = 0; i < TLB_SIZE; ++ i)
		if (TLB_LRU[i] > 0 && TLB_LRU[i] < TLB_LRU[pos])
			++ TLB_LRU[i];
	TLB_LRU[pos] = 1;
	return TLB_frame[pos];
}

void update_TLB(int page_number, int frame_number) {
	int pos = -1;
	for (int i = 0; i < TLB_SIZE; ++ i)
		if(TLB_LRU[i] == 0) {
			pos = i;
			break;
		}
	if (pos == -1) {
		for (int i = 0; i < TLB_SIZE; ++ i)
			if(TLB_LRU[i] == TLB_SIZE) {
				pos = i;
				break;
			}
	}
	
	TLB_page[pos] = page_number;
	TLB_frame[pos] = frame_number;
	for (int i = 0; i < TLB_SIZE; ++ i)
		if (TLB_LRU[i] > 0) ++ TLB_LRU[i];
	TLB_LRU[pos] = 1;
}
void delete_TLB(int page_number, int frame_number) {
	int pos = -1;
	for (int i = 0; i < TLB_SIZE; ++ i)
		if(TLB_LRU[i] && TLB_page[i] == page_number && TLB_frame[i] == frame_number) {
			pos = i;
			break;
		}	
	if (pos == -1) return;
	for (int i = 0; i < TLB_SIZE; ++ i)
		if (TLB_LRU[i] > TLB_LRU[pos])  TLB_LRU[i]--;
	TLB_LRU[pos] = 0;
}

void initialize() {
	page_fault_count = 0;
	for (int i = 0; i < PAGE_NUMBER; ++ i) {
		page_table[i] = 0;
		valid[i] = 0;
	}

	TLB_hit = 0;
	for (int i = 0; i < TLB_SIZE; ++ i) {
		TLB_page[i] = 0;
		TLB_frame[i] = 0;
		TLB_LRU[i] = 0;
	}

	backing_store = fopen("BACKING_STORE.bin", "rb");
	initialize_frame();
	for (int i = 0; i < FRAME_NUMBER; ++ i)
		LRU[i] = 0;
}

int get_frame_number(int page_number) {
	if (page_number < 0 || page_number >= PAGE_NUMBER) return -1;
	
	int TLB_res = get_TLB_frame_num(page_number);
	if (TLB_res != -1) return TLB_res;

	if (valid[page_number] == 1) {
		update_TLB(page_number, page_table[page_number]);
		return page_table[page_number];
	} else {
		++ page_fault_count;
		page_table[page_number] = add_page(page_number);
		valid[page_number] = 1;
		update_TLB(page_number, page_table[page_number]);
		return page_table[page_number];
	}
}

int main(int argc, char *argv[]) {	

	FILE *input_file = fopen(argv[1], "r");
	FILE *output_file = fopen("output.txt", "w");

	initialize();
		
	int address, page_number, offset, frame_number, result, count = 0;
	while(~fscanf(input_file, "%d", &address)) {
		++ count;
		address = address & 0x0000ffff;
		offset = address & 0x000000ff;
		page_number = (address >> 8) & 0x000000ff;
		frame_number = get_frame_number(page_number);
		result = (int) access_memory(frame_number, offset);
		fprintf(output_file, "Virtual address: %d Physical address: %d Value: %d\n", address, (frame_number << 8) + offset, result);
	}
	
	double TLB_hit_rate=100.0 * TLB_hit / count;
	double page_fault_rate=100.0 * page_fault_count / count;
	fprintf(stdout, "TLB hit rate is: %.1f %%\nPage fault rate is: %.1f %%\n", TLB_hit_rate, page_fault_rate);

	return 0;
}
