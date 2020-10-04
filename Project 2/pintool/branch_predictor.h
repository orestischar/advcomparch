#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

#include <sstream> // std::ostringstream
#include <cmath>   // pow()
#include <cstring> // memset()

/**
 * A generic BranchPredictor base class.
 * All predictors can be subclasses with overloaded predict() and update()
 * methods.
 **/
class BranchPredictor
{
public:
    BranchPredictor() : correct_predictions(0), incorrect_predictions(0) {};
    ~BranchPredictor() {};

    virtual bool predict(ADDRINT ip, ADDRINT target) = 0;
    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) = 0;
    virtual string getName() = 0;

    UINT64 getNumCorrectPredictions() { return correct_predictions; }
    UINT64 getNumIncorrectPredictions() { return incorrect_predictions; }

    void resetCounters() { correct_predictions = incorrect_predictions = 0; };

protected:
    void updateCounters(bool predicted, bool actual) {
        if (predicted == actual)
            correct_predictions++;
        else
            incorrect_predictions++;
    };

private:
    UINT64 correct_predictions;
    UINT64 incorrect_predictions;
};

class NbitPredictor : public BranchPredictor
{
public:
    NbitPredictor(unsigned index_bits_, unsigned cntr_bits_)
        : BranchPredictor(), index_bits(index_bits_), cntr_bits(cntr_bits_) {
        table_entries = 1 << index_bits;
        TABLE = new unsigned long long[table_entries];
        memset(TABLE, 0, table_entries * sizeof(*TABLE));
        
        COUNTER_MAX = (1 << cntr_bits) - 1;
    };
    ~NbitPredictor() { delete TABLE; };

    virtual bool predict(ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % table_entries;
        unsigned long long ip_table_value = TABLE[ip_table_index];
        unsigned long long prediction = ip_table_value >> (cntr_bits - 1);
        return (prediction != 0);
    };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % table_entries;
        if (actual) {
            if (TABLE[ip_table_index] < COUNTER_MAX)
                TABLE[ip_table_index]++;
        } else {
            if (TABLE[ip_table_index] > 0)
                TABLE[ip_table_index]--;
        }
        
        updateCounters(predicted, actual);
    };

    void update_without_count(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % table_entries;
        if (actual) {
            if (TABLE[ip_table_index] < COUNTER_MAX)
                TABLE[ip_table_index]++;
        } else {
            if (TABLE[ip_table_index] > 0)
                TABLE[ip_table_index]--;
        }
    };
    
    virtual string getName() {
        std::ostringstream stream;
        stream << "Nbit-" << pow(2.0,double(index_bits)) / 1024.0 << "K-" << cntr_bits;
        return stream.str();
    }

private:
    unsigned int index_bits, cntr_bits;
    unsigned int COUNTER_MAX;
    
    /* Make this unsigned long long so as to support big numbers of cntr_bits. */
    unsigned long long *TABLE;
    unsigned int table_entries;
};

class BTBEntry
{
    public:
        ADDRINT ip;
        ADDRINT target;
        UINT64 LRUCounter;

        BTBEntry()
            : ip(0), target(0), LRUCounter(0) {}

        BTBEntry(ADDRINT i, ADDRINT t, UINT64 cnt)
            : ip(i), target(t), LRUCounter(cnt) {}
};

class BTBPredictor : public BranchPredictor
{
private:

  unsigned int table_lines, table_assoc;
  unsigned int correct_target_predictions, incorrect_target_predictions;

  UINT64 timestamp;

  BTBEntry* TABLE;

  BTBEntry* find(ADDRINT ip) {
    unsigned int ip_table_index = ip % table_lines;

    for (unsigned int i = 0; i<table_assoc; i++) {

      BTBEntry* entry = &TABLE[ip_table_index*table_assoc+i];
      if (entry->ip == ip) {
        return entry;
      }
    }
    return NULL;
  }

  BTBEntry* replace(ADDRINT ip) {
    unsigned int ip_table_index = ip % table_lines;

    BTBEntry* entry;
    BTBEntry* LRUEntry = &TABLE[ip_table_index*table_assoc];

    for (unsigned int i = 0; i < table_assoc; i++) {
      entry = &TABLE[ip_table_index*table_assoc+i];

      if (LRUEntry->LRUCounter > entry->LRUCounter) {
        LRUEntry = entry;
      }
    }
    return LRUEntry;
  }

public:
    BTBPredictor(unsigned btb_lines, unsigned btb_assoc)
         : BranchPredictor(), table_lines(btb_lines), table_assoc(btb_assoc),
       correct_target_predictions(0), incorrect_target_predictions(0), timestamp(0) {

    TABLE = new BTBEntry[table_lines*table_assoc];
    }

    ~BTBPredictor() {
    delete TABLE;
    }

  virtual bool predict(ADDRINT ip, ADDRINT target) {

    BTBEntry* entry = find(ip);

    if (entry) {
      entry->LRUCounter = timestamp++;
      return true;
    }
        return false;
    }

  virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {

    if (actual && predicted) {

      BTBEntry* entry = find(ip);

      if (entry) {

        if (entry->target == target) {
          correct_target_predictions++;
        }
        else {
          incorrect_target_predictions++;
          entry->target = target;
        }

      }
      else {
        perror("BTB: Entry is not present although the branch is predicted taken");
      }
    }
    else if (actual && (!predicted)) {

      BTBEntry* entry = replace(ip);

      entry->ip = ip;
      entry->target = target;
      entry->LRUCounter = timestamp++;
    }
    else if ((!actual) && predicted) {

      BTBEntry* entry = find(ip);
      entry->ip = 0;
      entry->target = 0;
      entry->LRUCounter = 0;

    }

    updateCounters(predicted, actual);
    }

  virtual string getName() {
    std::ostringstream stream;
        stream << "BTB-" << table_lines << "-" << table_assoc;
        return stream.str();
    }

  UINT64 getNumCorrectTargetPredictions() {
        return correct_target_predictions;
    }

};


class StaticTakenPredictor : public BranchPredictor
{
public:
  StaticTakenPredictor():BranchPredictor() {};

  ~StaticTakenPredictor() {};

  virtual bool predict(ADDRINT ip, ADDRINT target) {
    return true;
  };

  virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
    updateCounters(predicted, actual);
  };

  virtual string getName() {
      std::ostringstream stream;
      stream << "StaticTakenPredictor";
      return stream.str();
  };

};

class BTFNTPredictor : public BranchPredictor
{
public:
  BTFNTPredictor ():BranchPredictor() {};
  ~BTFNTPredictor() {};

  virtual bool predict(ADDRINT ip, ADDRINT target) {
    return target < ip;
  };

  virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
    updateCounters(predicted, actual);
  };

  virtual string getName() {
      std::ostringstream stream;
      stream << "BTFNTPredictor";
      return stream.str();
  };

};

class TournamentPredictor : public BranchPredictor
{
public:
    TournamentPredictor(int _entries, BranchPredictor* A, BranchPredictor* B)
    : BranchPredictor(), entries(_entries)
    {
        PREDICTOR[0] = A;
        PREDICTOR[1] = B;

        // whatever, initial values are never used anyway
        prediction[0] = true;
        prediction[1] = true;

        // which one to use first -- arbitrary
        counter = new int[entries];
        memset(counter, 0, entries * sizeof(*counter));
    }

    virtual bool predict(ADDRINT ip, ADDRINT target) {
        int cnt = counter[ip % entries];

        prediction[0] = PREDICTOR[0]->predict(ip, target);
        prediction[1] = PREDICTOR[1]->predict(ip, target);

        // 0,1 -> prediction[0] --- 2,3 -> prediction[1]
        if (cnt < 2)
            return prediction[0];
        else if (cnt < 4)
            return prediction[1];
        else {
            cerr << "ERROR, SOMETHING WENT WRONG, TRUST NOTHING BEYOND THIS LINE" << endl;
            return false;
        }
    }

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        updateCounters(predicted, actual);
        int index = ip % entries;

        // update the actual predictors
        PREDICTOR[0]->update(prediction[0], actual, ip, target);
        PREDICTOR[1]->update(prediction[1], actual, ip, target);

        if (prediction[0] == prediction[1])
            return;

        if ((prediction[0] == actual) && (counter[index] > 0))
            counter[index] --; // favour p0 for this entry
        else if (counter[ip % entries] < 3)
            counter[index] ++; // favour p1 for this entry
    }

    virtual string getName() {
        std::ostringstream stream;
        stream << "Tournament(" << PREDICTOR[0]->getName() << "," << PREDICTOR[1]->getName() << ")";
        return stream.str();
    }

    ~TournamentPredictor()
    {
        delete counter;
    }

private:
    BranchPredictor *PREDICTOR[2];
    bool prediction[2];

    int entries;
    int *counter;
};

// local history predictor
class LocalHistoryPredictor : public BranchPredictor
{
public:
    LocalHistoryPredictor(int bht_entries_, int bht_bits_, int index_bits_, int nbits)
    : BranchPredictor(), pht_entries(1 << (index_bits_+bht_bits_)), pht_bits(nbits), bht_entries(bht_entries_), bht_bits(bht_bits_)
    {
        bhrmax = 1 << bht_bits;

        // create empty tables
        BHT = new int[bht_entries];
        for (int i = 0; i < bhrmax; i++) {
            PHT.push_back(new NbitPredictor(index_bits_, pht_bits));
        }
    }

    ~LocalHistoryPredictor() {
        PHT.clear();
        delete BHT;
    }

    // use the correct predictor using the local history
    virtual bool predict(ADDRINT ip, ADDRINT target) {
        int bhr = BHT[ip % bht_entries];
        return PHT[bhr]->predict(ip, target);
    }

    // update
    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        updateCounters(predicted, actual);

        // update predictor
        int & bhr = BHT[ip % bht_entries];
        PHT[bhr]->update(predicted, actual, ip, target);

        // update local history
        bhr = (bhr << 1) % bhrmax;      // shift left and drop MSB
        if (actual && bht_bits) bhr++;  // set LSB to 1 if branch was taken
    }

    virtual string getName() {
        ostringstream st;
        //int size = pht_entries * pht_bits + bht_entries*bht_bits;
        //size = size >> 10;
        st << "LocalHistory-PHT(" << pht_entries << "," << pht_bits << ")-BHT(" << bht_entries << "," << bht_bits << ")";
        return st.str();
    }

protected:
    int pht_entries, pht_bits;  // entries and bits per entry
    int bht_entries, bht_bits;  // entries and bits per entry

private:
    int *BHT;                   // use BHT[ip % bht_entries]
    vector<NbitPredictor*> PHT;  // use PHT[BHT[ip % bht_entries]]

    int bhrmax;                 // max entry for BHT
};

class GlobalHistoryPredictor : public LocalHistoryPredictor
{
public:
    GlobalHistoryPredictor(int _bhr_bits, int index_bits, int nbits)
    : LocalHistoryPredictor(1, _bhr_bits, index_bits, nbits)
    {}

    virtual string getName() {
        ostringstream st;
        //int size = pht_entries * pht_bits + bht_entries*bht_bits;
        //size = size >> 10;
        st << "GlobalHistory-PHT(" << pht_entries << "," << pht_bits << ")-BHR(" << bht_bits << ")";
        return st.str();
    }
};


class TwoLevelPredictor : public BranchPredictor
{
public:
    TwoLevelPredictor(unsigned index_bits_)
        : BranchPredictor(), index_bits(index_bits_) {
        table_entries = 1 << index_bits;
        TABLE = new unsigned long long[table_entries];
        memset(TABLE, 0, table_entries * sizeof(*TABLE));
        
        COUNTER_MAX = 3;
    };
    ~TwoLevelPredictor() { delete TABLE; };

   
    virtual bool predict(ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % table_entries;
        unsigned long long ip_table_value = TABLE[ip_table_index];
        unsigned long long prediction = ip_table_value >> 1;
        return (prediction != 0);
        /*if (ip_table_value == 0 || ip_table_value ==1)
          return false;
        else return true;*/
      };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % table_entries;
        if (actual) {
            if (TABLE[ip_table_index] == 0)
                TABLE[ip_table_index] = 1;
            else TABLE[ip_table_index] = 3;
        } else {
            if (TABLE[ip_table_index] == 3)
                TABLE[ip_table_index] = 2;
            else TABLE[ip_table_index] = 0;
        }
        
        updateCounters(predicted, actual);
    };
  
    virtual string getName(){
  std::ostringstream stream;
  stream << "TwoLevelPredictor" << pow(2.0,double(index_bits)) / 1024.0 << "K-" << 2;
  return stream.str();
    }

private:
    unsigned int index_bits;
    unsigned int COUNTER_MAX;
    
    /* Make this unsigned long long so as to support big numbers of cntr_bits. */
    unsigned long long *TABLE;
    unsigned int table_entries;
};

class LH2LevelPredictor : public BranchPredictor
{
public:
    LH2LevelPredictor(unsigned pht_bit_entries_, unsigned pht_length_, unsigned bht_bit_entries_, unsigned bht_length_)
        : BranchPredictor(), pht_entries(pht_bit_entries_), pht_length(pht_length_), bht_entries(bht_bit_entries_), bht_length(bht_length_)
        {
          bht_max = 1 << bht_bit_entries_;
        PHT = new NbitPredictor(pht_bit_entries_, pht_length_);
        BHT = new unsigned long long[bht_max];
        memset(BHT, 0, bht_max * sizeof(*BHT));
    };
    ~LH2LevelPredictor() { delete BHT; };

    virtual bool predict(ADDRINT ip, ADDRINT target) {
        int bht = BHT[ip % bht_max];
        int off = (ip & ((1 << (pht_entries - bht_length))-1)) << bht_length;
        int new_ip = bht | off;
        last_found = new_ip;
        return PHT->predict(new_ip, target);
    };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        updateCounters(predicted, actual);
        PHT->update(predicted, actual, last_found, target);
        BHT[ip % bht_max] = (BHT[ip % bht_max] << 1) % (1 << bht_length);
        if (actual) BHT[ip % bht_max]++;
    };

    virtual string getName() {
      ostringstream st;
      st << "LH2Level-" << pht_entries <<"-" << pht_length << "-" << bht_entries << "-" << bht_length;
      return st.str();
    };

protected:
    unsigned int  pht_entries, pht_length,bht_entries, bht_length, last_found, bht_max;

    /* Make this unsigned long long so as to support big numbers of cntr_bits. */
    unsigned long long *BHT;
    NbitPredictor *PHT;
};

class GHPredictor : public LH2LevelPredictor
{
public:
    GHPredictor(int pht_bits, int pht_length, int bht_length)
    : LH2LevelPredictor(pht_bits, pht_length, 1, bht_length)
    {}

      virtual unsigned long long history(int point){
       return BHT[point];
     };

    virtual string getName() {
        ostringstream st;
        st << "GlobalHistory:" << pht_entries <<"-"<<pht_length<< "-" << bht_entries << "-" << bht_length ;
        return st.str();
    }
};

class Alpha21264 : public BranchPredictor
{
public:
    Alpha21264(int global_history_bits,int global_history_length, int local_history_bht_entries, int local_history_bht_bits, int local_history_pht_bits)
    : BranchPredictor(), gh_bits(global_history_bits), gh_length(global_history_length), lh_bht_entries(local_history_bht_entries), lh_bht_bits(local_history_bht_bits),
    lh_pht_bits(local_history_pht_bits)
    {
        PREDICTOR[0] = new LH2LevelPredictor(local_history_bht_bits, local_history_pht_bits, local_history_bht_entries, local_history_bht_bits);
        PREDICTOR[1] = new GHPredictor(global_history_bits, global_history_length, global_history_bits);

        // whatever, initial values are never used anyway
        prediction[0] = true;
        prediction[1] = true;
        entries = 1 << global_history_bits;
        // which one to use first -- arbitrary
        counter = new int[entries];
        memset(counter, 0, entries * sizeof(*counter));
    }

    virtual bool predict(ADDRINT ip, ADDRINT target) {
        unsigned long long historyip =((GHPredictor*) PREDICTOR[1])->history(0);
        int cnt = counter[historyip];
        prediction[0] = PREDICTOR[0]->predict(ip, target);
        prediction[1] = PREDICTOR[1]->predict(historyip, target);

        // 0,1 -> prediction[0] --- 2,3 -> prediction[1]
        if (cnt < 2)
            return prediction[0];
        else if (cnt < 4)
            return prediction[1];
        else {
            cerr << "ERROR, SOMETHING WENT WRONG, TRUST NOTHING BEYOND THIS LINE" << endl;
            return false;
        }
    }

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        updateCounters(predicted, actual);
        unsigned long long index =((GHPredictor*) PREDICTOR[1])->history(0);

        // update the actual predictors
        PREDICTOR[0]->update(prediction[0], actual, ip, target);
        PREDICTOR[1]->update(prediction[1], actual, index, target);

        if (prediction[0] == prediction[1])
            return;

        if ((prediction[0] == actual) && (counter[index] > 0))
            counter[index] --; // favour p0 for this entry
        else if (counter[index] < 3)
            counter[index] ++; // favour p1 for this entry
    }

    virtual string getName() {
        std::ostringstream stream;
        stream << "Alpha21264-" << entries << "entries-(" << PREDICTOR[0]->getName() << "," << PREDICTOR[1]->getName() << ")";
        return stream.str();
    }

    ~Alpha21264()
    {
        delete counter;
    }

private:
  int gh_bits, gh_length, lh_bht_entries, lh_bht_bits, lh_pht_bits, *counter, entries;
    BranchPredictor *PREDICTOR[2];
    bool prediction[2];
};



#endif