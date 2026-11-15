//
//  Utils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-09.
//

import AVFoundation
import Foundation
import os

nonisolated let sameCharacterString = "－"
nonisolated let logger = Logger(
  subsystem: Bundle.main.bundleIdentifier!,
  category: String(describing: LogStore.self)
)
nonisolated let iso693ToLanguageName: [String: String] = [
  "cmn": "Mandarin",
  "deu": "German",
  "eng": "English",
  "fra": "French",
  "yue": "Cantonese",
  "zh": "Chinese",
  "zho": "Chinese",
]

nonisolated func unfoldBasicRegex(for pattern: String) -> [String] {
  var out: [String] = []
  var stringPossibilities: [String] = []

  // Invariant: there must be only one set of parentheses and one question mark
  if pattern.count(where: { $0 == "(" }) > 1
    || pattern.count(where: { $0 == ")" }) > 1
    || pattern.count(where: { $0 == "?" }) > 1
  {
    return out
  }

  // If there is a parenthesis, then all of the options must be checked
  if pattern.firstIndex(of: "(") != nil
    && pattern.firstIndex(of: ")") != nil
  {
    let openParenIdx = pattern.firstIndex(of: "(")
    var startIdx = pattern.index(after: openParenIdx!)
    let endIdx = pattern.firstIndex(of: ")")
    var orIdx = pattern.firstIndex(of: "|")

    while orIdx != nil {
      let preParen = pattern[..<openParenIdx!]
      let possibility = pattern[startIdx..<orIdx!]
      let postParen = pattern[pattern.index(after: endIdx!)...]
      stringPossibilities.append(
        String(preParen + possibility + postParen)
      )
      startIdx = pattern.index(after: orIdx!)
      orIdx = pattern.firstIndex(of: "|", at: startIdx)
    }
    let preParen = pattern[..<openParenIdx!]
    let possibility = pattern[startIdx..<endIdx!]
    let postParen = pattern[pattern.index(after: endIdx!)...]
    stringPossibilities.append(
      String(preParen + possibility + postParen)
    )
  } else {
    stringPossibilities.append(pattern)
  }

  // If there is a "?", then the initial with and the initial without
  // that optional character should be considered
  for s in stringPossibilities {
    let regexIdx = s.firstIndex(of: "?")
    if regexIdx == nil {
      out.append(s)
    } else if regexIdx == s.startIndex {
      out.append(String(s[s.index(after: s.startIndex)...]))
    } else {
      var tmp = s
      tmp.removeSubrange(
        s.index(before: regexIdx!)..<s.index(after: regexIdx!)
      )
      out.append(tmp)
      tmp = s
      tmp.remove(at: regexIdx!)
      out.append(tmp)
    }
  }

  return out
}

// Nabbed from https://forums.swift.org/t/find-multiple-substrings-from-specified-index/28667/4
extension String {
  nonisolated func firstIndex(of: String, at: String.Index) -> String.Index? {
    self[at...].range(of: of)?.lowerBound
  }
}

extension AVAudioPCMBuffer {
  nonisolated func deepCopy() -> AVAudioPCMBuffer? {
    guard let dest = AVAudioPCMBuffer(pcmFormat: format, frameCapacity: frameLength) else {
      return nil
    }
    dest.frameLength = frameLength

    let src = UnsafeMutableAudioBufferListPointer(mutableAudioBufferList)
    let dst = UnsafeMutableAudioBufferListPointer(dest.mutableAudioBufferList)
    for (s, d) in zip(src, dst) {
      guard let sData = s.mData, let dData = d.mData else {
        return nil
      }
      memcpy(dData, sData, Int(s.mDataByteSize))
    }
    return dest
  }
}
