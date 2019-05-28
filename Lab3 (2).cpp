#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

string pathData = "data.txt";
string pathIndexes = "ind.txt";
const string textExample = "Text";

struct IndexRecord {
	int key;
	int index;

	IndexRecord(int key, int index) : key(key), index(index) {}
};
struct Record {
	int number;
	string text;

	Record(int number, string text) : number(number), text(text) {}
};

void putData(vector<Record*> &recordes, string path) { // перезаписує файл дататкст і вносить в нього записи першої табл
	ofstream out(path); // аут це файл з текстом

	if (!out.is_open()) { // файл не відкрився
		cerr << "Open failure" << endl;
		exit(1);
	}

	for (int i = 0; i < recordes.size(); i++) { // якщо відкрився ми виконуємо це
		out << recordes[i]->number << " " << recordes[i]->text << endl; // виводить намбер і текст
	}
	out.close(); // закриваємо файл після того як все запис
}

void putRecord(Record* record, string path) { // добавляю нову інфу в файл
	ofstream out(path, ios::app); // завдяки йому

	if (!out.is_open()) {
		cerr << "Open failure" << endl;
		exit(1);
	}

	out << record->number << " " << record->text << endl;

	out.close();
}

vector<Record*> getData(string path) { // фія отримуємо відновлені записи з файлу
	ifstream data(path);
	if (!data.is_open()) {
		cerr << "Open failure" << endl;
		exit(1);
	}

	vector<Record*> recordes;

	int temp_Number;
	string temp_Text;
	while (data >> temp_Number >> temp_Text) {
		Record *record = new Record(temp_Number, temp_Text);
		recordes.push_back(record);
	}

	data.close();
	return recordes;
}

void putIndexes(vector<IndexRecord*> &recordes, string path) { // вносить дані в файл інндекса
	ofstream out(path);

	if (!out.is_open()) {
		cerr << "Open failure" << endl;
		exit(1);
	}

	for (int i = 0; i < recordes.size(); i++) {
		out << recordes[i]->key << " " << recordes[i]->index << endl;
	}

	out.close();
}

vector<IndexRecord*> getIndexes(string path) { // якщо комп перестав працювати щоб знову отримати ці дані
	ifstream indfile(path);
	if (!indfile.is_open()) {
		cerr << "Open failure" << endl;
		exit(1);
	}

	vector<IndexRecord*> recordes;

	int temp_Key;
	int temp_Index;
	while (indfile >> temp_Key >> temp_Index) {
		IndexRecord *record = new IndexRecord(temp_Key, temp_Index);
		recordes.push_back(record);
	}

	indfile.close();
	return recordes;
}

int FindIndexByKey(int key, vector<IndexRecord*> &indexes, int left, int right) {
	int midd = 0;
	while (true)
	{
		midd = (left + right) / 2;

		if (key < indexes[midd]->key)       // если искомое меньше значения в ячейке
			right = midd - 1;				// смещаем правую границу поиска
		else if (key > indexes[midd]->key)  // если искомое больше значения в ячейке
			left = midd + 1;				 // смещаем левую границу поиска
		else								// иначе (значения равны)
			return midd;					// функция возвращает индекс ячейки

		if (left > right)					// если границы сомкнулись
			return -1;
	}
}

void addIndex(vector<IndexRecord*> &indexes, IndexRecord *indexRecord) { // бінарний пошук
	if (indexes.size() == 0) {
		indexes.insert(indexes.begin(), indexRecord);
		return;
	}

	int midd = 0;
	int left = 0;
	int right = indexes.size();

	while (true)
	{
		midd = (left + right) / 2;

		if (indexRecord->key < indexes[midd]->key) {
			right = midd - 1;
		}
		else if (indexRecord->key > indexes[midd]->key) {
			left = midd + 1;
		}
		else {
			left = midd;
			break;
		}

		if (left >= right) {
			break;
		}
	}

	if (left < indexes.size() && indexRecord->key > indexes[left]->key) {
		left = left + 1;
	}

	indexes.insert(indexes.begin() + left, indexRecord);
}

void addRecord(vector<Record*> &recordes, vector<IndexRecord*> &indexes, Record* record) { // додає один запис в табл і табл індексів
	IndexRecord *indexRecord = new IndexRecord(record->number, recordes.size());
	recordes.push_back(record);
	addIndex(indexes, indexRecord);

	putRecord(record, pathData);
	putIndexes(indexes, pathIndexes);
}

void generateData(vector<Record*> &recordes, vector<IndexRecord*> &indexes, int N) { // генерує намбер і текст + намбер
	for (int i = 0; i < N; i++) {
		int number = rand() % 10000;
		string text = textExample + to_string(number);

		Record *record = new Record(number, text);
		recordes.push_back(record);

		IndexRecord *indexRecord = new IndexRecord(number, i);
		addIndex(indexes, indexRecord);
	}
}

void displayByIndex(vector<Record*> &recordes, int index) {
	cout << "Display: " << index << " " << recordes[index]->number << " " << recordes[index]->text << endl;
}

void del(vector<Record*> &recordes, vector<IndexRecord*> &indexes, int number) {
	int index = FindIndexByKey(number, indexes, 0, indexes.size());
	if (index < 0) return;
	indexes.erase(indexes.begin() + index);
	putIndexes(indexes, pathIndexes);
}

void edit(vector<Record*> &recordes, vector<IndexRecord*> &indexes, int number, Record *newRecord) {
	int index = FindIndexByKey(number, indexes, 0, indexes.size());
	if (index < 0) return;
	int indexRec = indexes[index]->index;
	indexes.erase(indexes.begin() + index);

	recordes[indexRec]->number = newRecord->number;
	recordes[indexRec]->text = newRecord->text;

	IndexRecord *indexRecord = new IndexRecord(recordes[indexRec]->number, indexRec);
	addIndex(indexes, indexRecord);

	putIndexes(indexes, pathIndexes);
	putData(recordes, pathData);
}



int main()
{
	vector<Record*> recordes;
	vector<IndexRecord*> indexes;

	//Generate:
/*	generateData(recordes, indexes, 100);
	putData(recordes, pathData);
	putIndexes(indexes, pathIndexes);
*/

	recordes = getData(pathData);
	indexes = getIndexes(pathIndexes);

	//Add
	/*	Record *record = new Record(170, "Nataly");
		addRecord(recordes, indexes, record);
	*/
	//Add by User
	/*	int number;
		string text;
		cout << "Enter Number: ";
		cin>> number;
		cout<<endl;
		cout<<"Enter text: ";
		cin>>text;

			Record *record = new Record(number, text);
			addRecord(recordes, indexes, record);
	*/

	//Search by number
	/*int number;

	cout << "Enter Number: ";
	cin>> number;
	int index = FindIndexByKey(number, indexes, 0, indexes.size());
	displayByIndex(recordes, indexes[index]->index);
	*/


	//Delete:
	/*int number;
	cout << "Enter Number: ";
	cin>> number;
	del(recordes, indexes, number);
	*/


	//Edit:
/*	int numberEdit;
	int number;
	string text;
	cout << "Enter Number for edit: ";
	cin >> numberEdit;
	cout << endl;
	cout << "Enter new Number: ";
	cin >> number;
	cout << endl;
	cout << "Enter new text: ";
	cin >> text;
	Record *newRecord = new Record(number, text);
	edit(recordes, indexes, numberEdit, newRecord);*/
}
