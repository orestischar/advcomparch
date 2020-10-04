#ifndef TLB_H
#define TLB_H

#include <iostream>  // std::cout ...
#include <cstdlib>   // rand()

#include "globals.h"



typedef UINT64 TLB_STATS; // type of tlb hit/miss counters



/**
 * `TLB_TAG` class represents an address tag stored in a Tlb.
 * `INVALID_TLB_TAG` is used as an error on functions with TLB_TAG return type.
 **/
class TLB_TAG
{
  private:
    ADDRINT _tag;

  public:
    TLB_TAG(ADDRINT tag = 0) { _tag = tag; }
    bool operator==(const TLB_TAG &right) const { return _tag == right._tag; }
    operator ADDRINT() const { return _tag; }
};
TLB_TAG INVALID_TLB_TAG(-1);


/**
 * Everything related to Tlb sets
 **/
namespace TLB_SET
{

class LRU 
{
  protected:
    std::vector<TLB_TAG> _tags;
    UINT32 _associativity;

 public:
    LRU(UINT32 associativity = 8)
    {
        _associativity = associativity;
        _tags.clear();
    }

    VOID SetAssociativity(UINT32 associativity)
    {
        _associativity = associativity;
        _tags.clear();
    }
    UINT32 GetAssociativity() { return _associativity; }

    string Name() { return "LRU"; }
    
    UINT32 Find(TLB_TAG tag)
    {
        for (std::vector<TLB_TAG>::iterator it = _tags.begin();
             it != _tags.end(); ++it)
        {
            if (*it == tag) { // Tag found, lets make it MRU
                _tags.erase(it);
                _tags.push_back(tag);
                return true;
            }
        }

        return false;
    }

    TLB_TAG Replace(TLB_TAG tag)
    {
        TLB_TAG ret = INVALID_TLB_TAG;
        _tags.push_back(tag);
        if (_tags.size() > _associativity) {
            ret = *_tags.begin();
            _tags.erase(_tags.begin());
        }
        return ret;
    }
    
    VOID DeleteIfPresent(TLB_TAG tag)
    {
        for (std::vector<TLB_TAG>::iterator it = _tags.begin();
             it != _tags.end(); ++it)
        {
            if (*it == tag) { // Tag found, lets make it MRU
                _tags.erase(it);
                break;
            }
        }
    }
};

} // namespace TLB_SET


template <class SET>
class SINGLE_LEVEL_TLB
{
  public:
    typedef enum 
    {
        ACCESS_TYPE_LOAD,
        ACCESS_TYPE_STORE,
        ACCESS_TYPE_NUM
    } ACCESS_TYPE;

  private:
    enum {
        HIT = 0,
        MISS,
        ACCESS_RESULT_NUM
    };

    static const UINT32 HIT_MISS_NUM = 2;
    TLB_STATS _access[ACCESS_TYPE_NUM][HIT_MISS_NUM];
    UINT32 _latencies[ACCESS_RESULT_NUM];
    SET *_sets;
    const std::string _name;
    const UINT32 _entries;
    const UINT32 _pageSize;
    const UINT32 _associativity;

    // computed params
    const UINT32 _lineShift; // i.e., no of page offset bits
    const UINT32 _setIndexMask; // mask applied to get the set index

    TLB_STATS SumAccess(bool hit) const
    {
        TLB_STATS sum = 0;
        for (UINT32 accessType = 0; accessType < ACCESS_TYPE_NUM; accessType++)
            sum += _access[accessType][hit];
        return sum;
    }

    UINT32 NumSets() const { return _setIndexMask + 1; }

    // accessors
    UINT32 Entries() const { return _entries; }
    UINT32 PageSize() const { return _pageSize; }
    UINT32 Associativity() const { return _associativity; }
    UINT32 LineShift() const { return _lineShift; }
    UINT32 SetIndexMask() const { return _setIndexMask; }

    VOID SplitAddress(const ADDRINT addr, UINT32 lineShift, UINT32 setIndexMask,
                      TLB_TAG & tag, UINT32 & setIndex) const
    {
        tag = addr >> lineShift;
        setIndex = tag & setIndexMask;
        tag = tag >> FloorLog2(NumSets());
    }


  public:
    // constructors/destructors
    SINGLE_LEVEL_TLB(std::string name,
                UINT32 Entries, UINT32 PageSize, UINT32 Associativity,
                UINT32 HitLatency = 0, UINT32 MissLatency = 100);

    // Stats
    TLB_STATS TlbHits(ACCESS_TYPE accessType) const { return _access[accessType][true];}
    TLB_STATS TlbMisses(ACCESS_TYPE accessType) const { return _access[accessType][false];}
    TLB_STATS TlbAccesses(ACCESS_TYPE accessType) const { return TlbHits(accessType) + TlbMisses(accessType);}
    TLB_STATS TlbHits() const { return SumAccess(true);}
    TLB_STATS TlbMisses() const { return SumAccess(false);}
    TLB_STATS TlbAccesses() const { return TlbHits() + TlbMisses();}

    string StatsLong(string prefix = "") const;
    string PrintDetails(string prefix = "") const;

    UINT32 Access(ADDRINT addr, ACCESS_TYPE accessType);
};

template <class SET>
SINGLE_LEVEL_TLB<SET>::SINGLE_LEVEL_TLB(
                std::string name,
                UINT32 Entries, UINT32 PageSize, UINT32 Associativity,
                UINT32 HitLatency, UINT32 MissLatency)
  : _name(name),
    _entries(Entries),
    _pageSize(PageSize),
    _associativity(Associativity),
    _lineShift(FloorLog2(PageSize)),
    _setIndexMask((Entries / Associativity) - 1)
{

    // They all need to be power of 2
    ASSERTX(IsPowerOf2(_pageSize));
    ASSERTX(IsPowerOf2(_setIndexMask + 1));

    // Allocate space for the sets
    _sets = new SET[NumSets()];

    _latencies[HIT] = HitLatency;
    _latencies[MISS] = MissLatency;

    for (UINT32 i = 0; i < NumSets(); i++)
        _sets[i].SetAssociativity(_associativity);

    for (UINT32 accessType = 0; accessType < ACCESS_TYPE_NUM; accessType++)
    {
        _access[accessType][false] = 0;
        _access[accessType][true] = 0;
    }
}

template <class SET>
string SINGLE_LEVEL_TLB<SET>::StatsLong(string prefix) const
{
    const UINT32 headerWidth = 19;
    const UINT32 numberWidth = 12;

    string out;
    
    // Tlb stats first
    out += prefix + "Tlb Stats:" + "\n";

    for (UINT32 i = 0; i < ACCESS_TYPE_NUM; i++)
    {
        const ACCESS_TYPE accessType = ACCESS_TYPE(i);

        std::string type(accessType == ACCESS_TYPE_LOAD ? "Tlb-Load" : "Tlb-Store");

        out += prefix + ljstr(type + "-Hits:      ", headerWidth)
               + dec2str(TlbHits(accessType), numberWidth)  +
               "  " +fltstr(100.0 * TlbHits(accessType) / TlbAccesses(accessType), 2, 6) + "%\n";

        out += prefix + ljstr(type + "-Misses:    ", headerWidth)
               + dec2str(TlbMisses(accessType), numberWidth) +
               "  " +fltstr(100.0 * TlbMisses(accessType) / TlbAccesses(accessType), 2, 6) + "%\n";
     
        out += prefix + ljstr(type + "-Accesses:  ", headerWidth)
               + dec2str(TlbAccesses(accessType), numberWidth) +
               "  " +fltstr(100.0 * TlbAccesses(accessType) / TlbAccesses(accessType), 2, 6) + "%\n";
     
        out += prefix + "\n";
    }

    out += prefix + ljstr("Tlb-Total-Hits:      ", headerWidth)
           + dec2str(TlbHits(), numberWidth) +
           "  " +fltstr(100.0 * TlbHits() / TlbAccesses(), 2, 6) + "%\n";

    out += prefix + ljstr("Tlb-Total-Misses:    ", headerWidth)
           + dec2str(TlbMisses(), numberWidth) +
           "  " +fltstr(100.0 * TlbMisses() / TlbAccesses(), 2, 6) + "%\n";

    out += prefix + ljstr("Tlb-Total-Accesses:  ", headerWidth)
           + dec2str(TlbAccesses(), numberWidth) +
           "  " +fltstr(100.0 * TlbAccesses() / TlbAccesses(), 2, 6) + "%\n";
    out += "\n";



    return out;
}

template <class SET>
string SINGLE_LEVEL_TLB<SET>::PrintDetails(string prefix) const
{
    string out;

    out += prefix + "--------\n";
    out += prefix + _name + "\n";
    out += prefix + "--------\n";
    out += prefix + "  Data Tlb:\n";
    out += prefix + "    Entries:       " + dec2str(this->Entries(), 5) + "\n";
    out += prefix + "    Page Size(B):  " + dec2str(this->PageSize(), 5) + "\n";
    out += prefix + "    Associativity:  " + dec2str(this->Associativity(), 5) + "\n";
    out += prefix + "\n";

    out += prefix + "Latencies: " + dec2str(_latencies[HIT], 4) + " "
                                  + dec2str(_latencies[MISS], 4) + "\n";
    out += prefix + "Tlb-Sets: " + dec2str(this->NumSets(), 4) + " - " + this->_sets[0].Name() + " - assoc: " +
                          dec2str(this->_sets[0].GetAssociativity(), 3) + "\n";
    out += "\n";

    return out;
}

// Returns the cycles to serve the request.
template <class SET>
UINT32 SINGLE_LEVEL_TLB<SET>::Access(ADDRINT addr, ACCESS_TYPE accessType)
{
    TLB_TAG Tag;
    UINT32 SetIndex;
    bool Hit = 0;
    UINT32 cycles = 0;

    // Check the TLB
    SplitAddress(addr, LineShift(), SetIndexMask(), Tag, SetIndex);
    SET & Set = _sets[SetIndex];
    Hit = Set.Find(Tag);
    _access[accessType][Hit]++;
    cycles = _latencies[HIT];

    if (!Hit) {
        // On miss, allocate
        Set.Replace(Tag);
        cycles = _latencies[MISS];
    }

    return cycles;
}

#endif // TLB_H
