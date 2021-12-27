# Slow pace mode

The idea described quite well, by Rymdkejsaren1, in [his github comment](https://github.com/OpenTTD/OpenTTD/discussions/8397#discussioncomment-424935):

> We just want a slower general pace, with the years progressing considerably slower so that we can log in, build a line, and not come back a day later to massive cities and someone has made an absolute killing on their two oil lines that now have been running for decades.

And finally [there is a version](https://github.com/kaomoneus/OpenTTD/releases/tag/12.1.slowpace-1.1) which works exactly this way. It even supports multiplayer.

There might be some bugs , of course and everyone is welcome to test it. Just download client, open multiplayer and find
"Daily pace" server. Here is invitation:
```
+5gX5VdJ
```

[Here, in github bug tracker](https://github.com/kaomoneus/OpenTTD/issues) you can create bugs/tasks/whatever.

First you might watch short feature overview (~6mins):

[![Video: OpenTTD Slow Pace](https://img.youtube.com/I8bn7Gb7gu8/0.jpg)](https://www.youtube.com/watch?v=I8bn7Gb7gu8)

Or opposite. If you're lazy to read human friendly text, there is a [`slow_pace`](https://github.com/kaomoneus/OpenTTD/commits/slow_pace) branch where all commits reflect
particular subfeature of *Slow Pace Mode*. All changes gathered in only 10 commits.

Otherwise though, below you can find intuitive description and even few C++ implementation details.

## Some intuitive implementation description
Key point is to split notion of "day" onto *animation day* and *game day*. So we have two time streams:

* animation time
* game time
  
And this is generally how you can watch ages changing slower than usual and animation going at regular pace. All changes in this fork were about sorting mechs between "animation" or "game" , or sometimes somewhere in the middle.

### Animation day (aka "*vanilla day*")
This is a time unit all animation is bound to. There are other game mechs, which are also tied mostly to *animation days*, for example:
* vehicles moving 
* tiles animation.
* station ratings (not that obvious though)

### Game day
"Game time" metric is about how rapidly you're able to become a big boss (or a bankrupt). This kind of "time" is responsible for game "turning point" events. Just few examples of mechs tied to *game days*:

* income growth
* advancement of technoligy
* airplane crashes and other disasters
* towns and cities growth rate

That's it. This split up is a key point of all changes. Below is an implementation description. Mostly it covers things on quite a high level, but sometimes I'll mention some source code fragments.

## Technical implementation aspects (non-intuitive part)

### Game balance
...is everything. It is top priority to keep it unchanged.

So if for a single bus *payback period* is ~2.5 game years. It should remain unchanged.

If in vanilla version you can return loan in 3-6 years after game started, it still should be same in slow pace mode.

And so on.

### Ticks and days
First of all `DAY_TICKS` became a *variable* which is set when you start new game. So user can select how fast the game pace should be.

In some places (especially *animation*) we still need *vanilla day*. So `VANILLA_DAY_TICKS` was introduced. This constant is equal to 74. If user plays game at regular pace, then `DAY_TICKS === VANILLA_DAY_TICKS`

There are plenty of trivial changes, where we just replace `DAY_TICKS` with `VANILLA_DAY_TICKS`.

As a consequencies some fields changed their bitwidth from 8 bits to 16 bits and so on. E.g. `_date_fract` was converted into `uint16`.

We describe such changes a bit later.

#### `Pace factor`
Sometimes this term is used to indicate the relation between vanilla pace and current game pace. So:

```
<Pace Factor> = DAY_TICKS / VANILLA_DAY_TICKS
```

### Affected mechanics
Below is brief description of all mechs affected by this change.

#### Date -> Date time
Just because *day* in some modes becomes too rough to measure time *hours and minutes* were introduced in some dialogs.

And of course date indication in main bottom panel has been replaced with *date + time*.

#### Cargo payment rates
This is the first non-trivial case. In fact this mech is tied more to *animation* pace.

In another words, if I get bus full of passengers running from one corner of 4k map to another one, I should get the lowest possible earning no matter whether I'm playing slow pace or vanilla pace.

Thus whenever we resolve *cargo payment amount* among with distance and original fares we also take into account *pace factor*.

In fact changes only affect a *Graph Dialog*. In game engine itself it is `transit_days` which reflects delivery time and is strictly bound to ticks (see `AgeCargo` method), we can just treat them as *transit vanilla days*. Then it is totally fine for us, to keep this part unchanged.

But just to be clear, this is what we have on output (even without changing mech itself):

Vanilla version:
```c++
profit = GetTransportedGoodsIncome(..., days_in_transit, ...);
```

Slow pace version:

```c++
// This code doesn't exist, it just reflects
// implicit calculation.
// day_in_transit is what user can observe watching date and time
// in main bottom panel.
vanilla_days_in_transit = pace_factor * days_in_transit;

profit = GetTransportedGoodsIncome(..., days_in_transit, ...)
```

We divide game time onto *pace factor*, and this is how we compensate game slow down.

So 200 days period on "cargo payment rates" graph might be ~3000 minutes in daily pace mode (1 game year = 1 user's day).

#### Breakdowns, service, Vehicle Dialog
Same here. You can't just send bus from one corner of big map to another one without visiting a depo from time to time.

Breakdown probability multiplied on *pace factor*.

Vehicle dialog also was updated so in slow pace modes you could (and you must) send vehicle to depo every several hours and even may be minutes. Otherwise it will be broken.

And this is may be most drammatic change. In order to bring it all to live `OnNewVanillaDay` event was introduced, it triggers breakdowns instead of `OnNewDay` as it was in vanilla version.

#### Airplane crashes
It's a bit opposite to breakdowns. As it was mentiong, the game balance is everything. Whenever airplane crashes you need some time to get money for a new one. So airplane crashes should happen at *game pace*.

In vanilla version crashes and probability calculation were triggered by *tick* event. It still works same way, but we propotionally changed the probability formulae for slow paces.

#### Network
Network is tied to animation time. Here I just have added few dozens of lines in order to debug desyncs. All except `DAY_TICKS` -> `VANILLA_DAY_TICKS` replacement might be rejected.

#### Money and cargo growth rate
Purpose was to be able to return all loans in same period, and to develop company at same internal-game-pace. E.g. in average you should be able to become a big-boss in ~10-15 years after game started.

And here is a trick.

* I didn't change transfer fares.
* All purchase costs multiplied on *pace factor*
* Initial loan and maximum possible loan also was increased <*pace factor*> times

So vehicle earn *exactly same* amount per trip as it was in vanilla version. But they must run more trips to cover cost of their own. Namely they should run <*pace factor*> times more trips.

And this is how you still should have bus working 2.5 game years to cover cost of its own.

##### Cargo growth, station ratings, orderchecks, resorts

It is tied to tick, so in another words in original game it was tied to animation. And it's OK.

In slow pace mode we need more trips to cover costs, and thus we need more cargo to be transfered!

So it's OK to keep this growth rate mech as it is now.

Same with station ratings, order checks and route resorts, its's totally fine to keep them bound to game ticks.

#### Town growth

Readjusted to respect selected game pace.

#### Timetables

This mech internally uses 'ticks' to configure and follow timetables. And this is OK, because vehicles run is bound to animation day.

But, Timetables Dialog was extened with hours and minutes for slow pace modes. So, you can set timetable start at exact hour and minute when playing slow pace. You also can observe arrival and departure dates.. and times.

#### Save/load and bit widths of some game state fields
As it was mentioned `DAY_TICKS` in vanilla version was equal to 74 and it was enough to use 8 bits to store most of tick related fields.

But I want be able to slowdown game like 100 times or even 1000 times. For example if we want game year to last one user's real day, we need to slow increase this constat ~96 times. If I want year pace to last user's week, I need to slowdown game ~672 times.

So that was the reason why some fields were converted from 8 bit integers to 16 bit and so on.

Below are extended fields:

* `CompanyProperties::bankrupt_timeout`: 16 bits -> 32 bits
* `_tick_counter`: 16 bits -> 32 bits.
* `LoggedAction::tick`: 16 bits -> 32 bits.
* `Industry::produced_cargo_waiting, incoming_cargo_waiting`: 16 bits -> 32 bits. There are *clamps* made with `std::min` like this:
    ```c++
    i->produced_cargo_waiting[0] = std::min(0xffff, i->produced_cargo_waiting[0] + 45);
    ```

    All constructions like above were updated to respect new bit width. Namely this one has been replaced with:
    ```c++
    i->produced_cargo_waiting[0] = std::min(0xffffffff, i->produced_cargo_waiting[0] + 45);
    ```
* `CargoInfo::production`: 16 bits -> 32 bits.
* `NewGRFProfiler::tick`: 16 bits -> 32 bits.
* `Town::time_until_rebuild, grow_counter, growth_rate`: bit width doubled.
* `Vehicle::running_ticks`: 8 bits -> 32 bits. 

#### API: New GRF and scripts
For slow pace modes it's not enough for example to know how many days takes to do something. You also need to now the fracture of days. It generally works. But for example service period can't be requested at desired precision for slow pace modes.

But again, in general it works.

## Conclusion
To me "slow pace" is an only way to play this game, otherwise game becomes a time-killer thing. I just can't stop playing it, haha! Hopefully it help others and may be gives some ideas to devs how to apply and whether it is worth applying to mainstream.

I invite everybody to test this addition. Here are links to release and server invitations:

* [OpenTTD Slow Pace project](https://github.com/kaomoneus/OpenTTD/)
* [OpenTTD 12.1.slowpace-1.1 Release](https://github.com/kaomoneus/OpenTTD/releases/tag/12.1.slowpace-1.1)
* Daily server invitation (one game year is ~1 user's day): +5gX5VdJ
* [OpenTTD Slow Pace Bugs/tasks tracker](https://github.com/kaomoneus/OpenTTD/issues)
