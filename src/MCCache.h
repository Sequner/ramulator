#ifndef __MCCACHE_H
#define __MCCACHE_H

#include "Config.h"
#include "Request.h"
#include "Statistics.h"
#include <algorithm>
#include <cstdio>
#include <cassert>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <list>

namespace ramulator
{

class MCCache {
protected:
  ScalarStat cache_read_miss;
  ScalarStat cache_write_miss;
  ScalarStat cache_total_miss;
  ScalarStat cache_eviction;
  ScalarStat cache_read_access;
  ScalarStat cache_write_access;
  ScalarStat cache_total_access;
  ScalarStat cache_mshr_hit;
  ScalarStat cache_mshr_unavailable;
  ScalarStat cache_set_unavailable;
public:
  struct Line {
    long addr;
    long tag;
    bool lock; // When the lock is on, the value is not valid yet.
    bool dirty;
    Line(long addr, long tag):
        addr(addr), tag(tag), lock(true), dirty(false) {}
    Line(long addr, long tag, bool lock, bool dirty):
        addr(addr), tag(tag), lock(lock), dirty(dirty) {}
  };

  MCCache(int size, int assoc, int block_size, int mshr_entry_num, int channel_num);
  void helloworld() {
    printf("helloworld");
  }; 
  void tick();

  // Cache latency
  int latency = 2;

  bool send(Request req);

  void callback(Request& req);

  size_t size;
  unsigned int assoc;
  unsigned int block_num;
  unsigned int index_mask;
  unsigned int block_size;
  unsigned int index_offset;
  unsigned int tag_offset;
  unsigned int mshr_entry_num;
  unsigned int channel_num;
  std::vector<std::pair<long, std::list<Line>::iterator>> mshr_entries;

  std::map<int, std::list<Line> > cache_lines;

  int calc_log2(int val) {
      int n = 0;
      while ((val >>= 1))
          n ++;
      return n;
  }

  int get_index(long addr) {
    return (addr >> index_offset) & index_mask;
  };

  long get_tag(long addr) {
    return (addr >> tag_offset);
  }

  // Align the address to cache line size
  long align(long addr) {
    return (addr & ~(block_size-1l));
  }

  // Evict the victim from current set of lines.
  // First do invalidation, then call evictline(L1 or L2) or send
  // a write request to memory(L3) when dirty bit is on.
  void evict(std::list<Line>* lines,
      std::list<Line>::iterator victim);

  // Allocate a cache line upon issuing read request through scheduler
  void allocate_mshr(Request req);

  // First test whether need eviction, if so, do eviction by
  // calling evict function. Then allocate a new line and return
  // the iterator points to it.
  std::list<Line>::iterator allocate_line(
      std::list<Line>& lines, long addr, bool lock, bool dirty);

  // Check whether the set to hold addr has space or eviction is
  // needed.
  bool need_eviction(const std::list<Line>& lines, long addr);

  // Check whether this addr is hit and fill in the pos_ptr with
  // the iterator to the hit line or lines.end()
  bool is_hit(std::list<Line>& lines, long addr,
              std::list<Line>::iterator* pos_ptr);

  bool all_sets_locked(const std::list<Line>& lines) {
    if (lines.size() < assoc) {
      return false;
    }
    for (const auto& line : lines) {
      if (!line.lock) {
        return false;
      }
    }
    return true;
  }

  bool check_unlock(long addr) {
    auto it = cache_lines.find(get_index(addr));
    if (it == cache_lines.end()) {
      return true;
    } else {
      auto& lines = it->second;
      auto line = find_if(lines.begin(), lines.end(),
          [addr, this](Line l){return (l.tag == get_tag(addr));});
      if (line == lines.end()) {
        return true;
      }
      return !line->lock;
    }
  }

  std::vector<std::pair<long, std::list<Line>::iterator>>::iterator
  hit_mshr(long addr) {
    auto mshr_it =
        find_if(mshr_entries.begin(), mshr_entries.end(),
            [addr, this](std::pair<long, std::list<Line>::iterator>
                   mshr_entry) {
              return (align(mshr_entry.first) == align(addr));
            });
    return mshr_it;
  }

  std::list<Line>& get_lines(long addr) {
    if (cache_lines.find(get_index(addr))
        == cache_lines.end()) {
      cache_lines.insert(make_pair(get_index(addr),
          std::list<Line>()));
    }
    return cache_lines[get_index(addr)];
  }

  // wait_list contains miss requests with their latencies in
  // cache. When this latency is met, the send_memory function
  // will be called to send the request to the memory system.
  std::vector<std::list<std::pair<long, Request> > > ld_wait_list;
  std::list<std::pair<long, Request> > st_wait_list;

  // hit_list contains hit requests with their latencies in cache.
  // callback function will be called when this latency is met and
  // set the instruction status to ready in processor's window.
  std::vector<std::list<std::pair<long, Request> > > hit_list;

  std::function<bool(Request)> send_memory = NULL;

  long clk = 0;

};

} // namespace ramulator

#endif /* __CACHE_H */
