//
//  DatabaseDocument.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-28.
//

import SwiftUI
import UniformTypeIdentifiers
import os

struct DatabaseDocument: FileDocument {
  static var readableContentTypes: [UTType] { [.database] }
  let url: URL

  init(url: URL) {
    self.url = url
  }

  init(configuration: ReadConfiguration) throws {
    logger.error("Attempted to read DatabaseDocument, not supported!")
    fatalError("DatabaseDocument read not supported!")
  }

  func fileWrapper(configuration: WriteConfiguration) throws -> FileWrapper {
    try FileWrapper(url: url, options: .immediate)
  }

}
