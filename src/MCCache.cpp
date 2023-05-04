#include "MCCache.h"

#ifndef DEBUG_CACHE
#define debug(...)
#else
#define debug(...) do { \
          printf("\033[36m[DEBUG] %s ", __FUNCTION__); \
          printf(__VA_ARGS__); \
          printf("\033[0m\n"); \
      } while (0)
#endif

namespace ramulator
{

MCCache::MCCache(int size, int assoc, int block_size,
    int mshr_entry_num, int channel_num):
    size(size), assoc(assoc),
    block_size(block_size), 
    mshr_entry_num(mshr_entry_num), channel_num(channel_num) {

    debug("MC_Cache size %d assoc %d block_size %d\n",
        size, assoc, block_size);

    // Check size, block size and assoc are 2^N
    assert((size & (size - 1)) == 0);
    assert((block_size & (block_size - 1)) == 0);
    assert((assoc & (assoc - 1)) == 0);
    assert(size >= block_size);

    for (int i = 0; i < channel_num; i++) {
        std::list<std::pair<long, Request>> mylist1, mylist2, mylist3;
        ld_wait_list.push_back(mylist1);
        hit_list.push_back(mylist3);
    }

    // Initialize cache configuration
    block_num = size / (block_size * assoc);
    index_mask = block_num - 1;
    index_offset = calc_log2(block_size);
    tag_offset = calc_log2(block_num) + index_offset;

    debug("index_offset %d", index_offset);
    debug("index_mask 0x%x", index_mask);
    debug("tag_offset %d", tag_offset);

    // regStats
    cache_read_miss.name(string("MC_cache_read_miss"))
                    .desc("cache read miss count")
                    .precision(0)
                    ;

    cache_write_miss.name(string("MC_cache_write_miss"))
                    .desc("cache write miss count")
                    .precision(0)
                    ;

    cache_total_miss.name(string("MC_cache_total_miss"))
                    .desc("cache total miss count")
                    .precision(0)
                    ;

    cache_eviction.name(string("MC_cache_eviction"))
                    .desc("number of evict from this level to lower level")
                    .precision(0)
                    ;

    cache_read_access.name(string("MC_cache_read_access"))
                    .desc("cache read access count")
                    .precision(0)
                    ;

    cache_write_access.name(string("MC_cache_write_access"))
                        .desc("cache write access count")
                        .precision(0)
                        ;

    cache_total_access.name(string("MC_cache_total_access"))
                        .desc("cache total access count")
                        .precision(0)
                        ;

    cache_mshr_hit.name(string("MC_cache_mshr_hit"))
                    .desc("cache mshr hit count")
                    .precision(0)
                    ;
    cache_mshr_unavailable.name(string("MC_cache_mshr_unavailable"))
                            .desc("cache mshr not available count")
                            .precision(0)
                            ;
    cache_set_unavailable.name(string("MC_cache_set_unavailable"))
                            .desc("cache set not available")
                            .precision(0)
                            ;
}

bool MCCache::send(Request req) {
    debug("MC_Cache req.addr %lx req.type %d, index %d, tag %ld",
        req.addr, int(req.type), get_index(req.addr),
        get_tag(req.addr));

    cache_total_access++;
    if (req.type == Request::Type::WRITE) {
        cache_write_access++;
    } else {
        assert(req.type == Request::Type::READ);
        cache_read_access++;
    }
    // If there isn't a set, create it.
    auto& lines = get_lines(req.addr);
    std::list<Line>::iterator line;

    if (is_hit(lines, req.addr, &line)) {
        lines.push_back(Line(req.addr, get_tag(req.addr), false,
            line->dirty || (req.type == Request::Type::WRITE))); // LRU is used, so line is pushed back
        lines.erase(line);
        req.depart = clk + latency;
        debug("hit, update timestamp %ld", clk);
        debug("hit finish time %ld", clk + latency);

        return true;

    } else {
        debug("miss @MC_Cache");
        cache_total_miss++;
        if (req.type == Request::Type::WRITE) {
            cache_write_miss++;
            // Check whether there is a line available
            if (all_sets_locked(lines)) {
                cache_set_unavailable++;
                return false;
            }

            auto newline = allocate_line(lines, req.addr, false, true);
            if (newline == lines.end()) {
                return false;
            }
            return true;
        }

        cache_read_miss++;

        // WRITE Queue lookup is done in controller

        // Send the request to next level;
        ld_wait_list[req.addr_vec[0]].push_back(make_pair(clk + latency, req));
        return true;
    }
}

void MCCache::evict(std::list<Line>* lines,
    std::list<Line>::iterator victim) {
    debug("MC Cache miss evict victim %lx", victim->addr);
    cache_eviction++;

    long addr = victim->addr;
    long invalidate_time = 0;
    bool dirty = victim->dirty;

    // LLC eviction
    if (dirty) {
        Request write_req(addr, Request::Type::WRITE);
        write_req.arrive = clk;
        st_wait_list.push_back(make_pair(
            clk + invalidate_time + latency,
            write_req));

        debug("inject one write request to memory system "
            "addr %lx, invalidate time %ld, issue time %ld",
            write_req.addr, invalidate_time,
            clk + invalidate_time + latency);
    }

    lines->erase(victim);
}

void MCCache::allocate_mshr (Request req) {
    auto& lines = get_lines(req.addr);
    std::list<Line>::iterator line;
    auto newline = allocate_line(lines, req.addr, false, false);
    if (newline == lines.end()) {
        printf("Could allocate new cache line");
        assert(false); // shouldn't happen
    }
}

std::list<MCCache::Line>::iterator MCCache::allocate_line(
    std::list<Line>& lines, long addr, bool lock, bool dirty) {
    // See if an eviction is needed
    if (need_eviction(lines, addr)) {
        // Get victim.
        // The first one might still be locked due to reorder in MC
        auto victim = find_if(lines.begin(), lines.end(),
            [this](Line line) { 
                return !line.lock;
            });
        if (victim == lines.end()) {
            return victim;  // doesn't exist a line that's already unlocked in each level
        }
        assert(victim != lines.end());
        evict(&lines, victim);
    }

    // Allocate newline, with lock bit on and dirty bit off
    lines.push_back(Line(addr, get_tag(addr), lock, dirty));
    auto last_element = lines.end();
    --last_element;
    return last_element;
}

bool MCCache::is_hit(std::list<Line>& lines, long addr,
    std::list<Line>::iterator* pos_ptr) {
    auto pos = find_if(lines.begin(), lines.end(),
        [addr, this](Line l){return (l.tag == get_tag(addr));});
    *pos_ptr = pos;
    if (pos == lines.end()) {
        return false;
    }
    return !pos->lock;
}

bool MCCache::need_eviction(const std::list<Line>& lines, long addr) {
    if (find_if(lines.begin(), lines.end(),
        [addr, this](Line l){
            return (get_tag(addr) == l.tag);})
        != lines.end()) {
        // Due to MSHR, the program can't reach here. Just for checking
        assert(false);
    } else {
        if (lines.size() < assoc) {
            return false;
        } else {
            return true;
        }
    }
}

void MCCache::callback(Request& req) {
    debug("MC Cache Callback");

    auto it = find_if(mshr_entries.begin(), mshr_entries.end(),
        [&req, this](std::pair<long, std::list<Line>::iterator> mshr_entry) {
            return (align(mshr_entry.first) == align(req.addr));
        });

    if (it != mshr_entries.end()) {
        it->second->lock = false;
        mshr_entries.erase(it);
    }
}

void MCCache::tick() {
    debug("clk %ld", clk);

    ++clk;
    // send store
    auto it = st_wait_list.begin();
    while (it != st_wait_list.end() && clk >= it->first) {
        if (!send_memory(it->second)) {
            ++it;
        } else {

            debug("complete req: addr %lx", (it->second).addr);

            it = st_wait_list.erase(it);
        }
    }

}

} // namespace ramulator
