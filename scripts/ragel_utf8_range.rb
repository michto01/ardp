#!/usr/bin/env ruby

# Usage: ragel_utf8_range.rb <start> <end>
#
# This script is based on unicode2ragel.rb by Rakan El-Khalil <rakan@well.com>,
# which was originally written as part of the Ferret search engine library.

###
# Formats to hex at minimum width

def to_hex( n )
  r = "%0X" % n
  r = "0#{r}" unless (r.length % 2).zero?
  r
end

##
# 0x00     - 0x7f     -> 0zzzzzzz[7]
# 0x80     - 0x7ff    -> 110yyyyy[5] 10zzzzzz[6]
# 0x800    - 0xffff   -> 1110xxxx[4] 10yyyyyy[6] 10zzzzzz[6]
# 0x010000 - 0x10ffff -> 11110www[3] 10xxxxxx[6] 10yyyyyy[6] 10zzzzzz[6]

UTF8_BOUNDARIES = [0x7f, 0x7ff, 0xffff, 0x10ffff]

def to_utf8_enc( n )
  r = 0
  if n <= 0x7f
    r = n
  elsif n <= 0x7ff
    y = 0xc0 | (n >> 6)
    z = 0x80 | (n & 0x3f)
    r = y << 8 | z
  elsif n <= 0xffff
    x = 0xe0 | (n >> 12)
    y = 0x80 | (n >>  6) & 0x3f
    z = 0x80 |  n        & 0x3f
    r = x << 16 | y << 8 | z
  elsif n <= 0x10ffff
    w = 0xf0 | (n >> 18)
    x = 0x80 | (n >> 12) & 0x3f
    y = 0x80 | (n >>  6) & 0x3f
    z = 0x80 |  n        & 0x3f
    r = w << 24 | x << 16 | y << 8 | z
  end

  to_hex(r)
end

###
# Given a range, splits it up into ranges that can be continuously
# encoded into utf8.  Eg: 0x00 .. 0xff => [0x00..0x7f, 0x80..0xff]
# This is not strictly needed since the current [5.1] unicode standard
# doesn't have ranges that straddle utf8 boundaries.  This is included
# for completeness as there is no telling if that will ever change.

def utf8_ranges( range )
  ranges = []
  UTF8_BOUNDARIES.each do |max|
    if range.begin <= max
      return ranges << range if range.end <= max

      ranges << range.begin .. max
      range = (max + 1) .. range.end
    end
  end
  ranges
end

def build_range( start, stop )
  size = start.size/2
  left = size - 1
  return [""] if size < 1

  a = start[0..1]
  b = stop[0..1]

  ###
  # Shared prefix

  if a == b
    return build_range(start[2..-1], stop[2..-1]).map do |elt|
      "0x#{a} " + elt
    end
  end

  ###
  # Unshared prefix, end of run

  return ["0x#{a}..0x#{b} "] if left.zero?

  ###
  # Unshared prefix, not end of run
  # Range can be 0x123456..0x56789A
  # Which is equivalent to:
  #     0x123456 .. 0x12FFFF
  #     0x130000 .. 0x55FFFF
  #     0x560000 .. 0x56789A

  ret = []
  ret << build_range(start, a + "FF" * left)

  ###
  # Only generate middle range if need be.

  if a.hex+1 != b.hex
    max = to_hex(b.hex - 1)
    max = "FF" if b == "FF"
    ret << "0x#{to_hex(a.hex+1)}..0x#{max} " + "0x00..0xFF " * left
  end

  ###
  # Don't generate last range if it is covered by first range

  ret << build_range(b + "00" * left, stop) unless b == "FF"
  ret.flatten!
end

def to_utf8( range )
  utf8_ranges( range ).map do |r|
    build_range to_utf8_enc(r.begin), to_utf8_enc(r.end)
  end.flatten!
end


puts to_utf8( ARGV[0].hex .. (ARGV[1] != '..' ? ARGV[1].hex : ARGV[2].hex))
