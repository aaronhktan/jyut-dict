//
//  HandwritingView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import MLKitDigitalInkRecognition
import SwiftUI

struct HandwritingView: View {
  @Environment(\.dismiss) var dismiss

  @Bindable var searchContext: SearchContext

  @Binding var isSearchActive: Bool

  @State private var handwritingState = HandwritingState()
  @State private var searchText = ""
  @State private var buttonText = "Search"

  let lineWidth = 5.0
  let epsilon = 5.0

  var body: some View {
    NavigationStack {
      VStack {
        if handwritingState.recognizer == nil {
          ContentUnavailableView {
            ProgressView()
              .scaleEffect(2.0)
              .padding()
            Text("Downloading and installing handwriting model...")
          }
        } else {
          CanvasView()
          CandidatesView()
          Spacer()
          ButtonView()
        }
      }
      .toolbar {
        #if os(iOS)
          ToolbarItem(placement: .topBarTrailing) {
            Button("Close handwriting view", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #else
          Button("Close handwriting view", systemImage: "xmark") {
            dismiss()
          }
        #endif
      }
    }
    .task {
      let languageTag = "zh-Hani-HK"
      let identifier = DigitalInkRecognitionModelIdentifier(forLanguageTag: languageTag)
      guard let identifier else {
        print("Couldn't find identifier!")
        return
      }
      let model = DigitalInkRecognitionModel(modelIdentifier: identifier)
      let modelManager = ModelManager.modelManager()
      if !modelManager.isModelDownloaded(model) {
        let conditions = ModelDownloadConditions.init(
          allowsCellularAccess: true,
          allowsBackgroundDownloading: false)
        modelManager.download(model, conditions: conditions)
      }
      handwritingState.recognizer = DigitalInkRecognizer.digitalInkRecognizer(
        options: DigitalInkRecognizerOptions(model: model))
    }
  }

  func CGPointDistanceSquared(from: CGPoint?, to: CGPoint?) -> CGFloat? {
    if let from, let to {
      return (from.x - to.x) * (from.x - to.x) + (from.y - to.y) * (from.y - to.y)
    }

    return nil
  }
}

extension HandwritingView {
  // MARK: - Canvas
  @ViewBuilder
  func CanvasView() -> some View {
    Canvas { context, size in
      for path in handwritingState.paths {
        if path.isFilled {
          context.fill(path.path, with: .color(.primary))
        } else {
          context.stroke(path.path, with: .color(.primary), lineWidth: lineWidth)
        }
      }
      context.stroke(handwritingState.currentPath, with: .color(.primary), lineWidth: lineWidth)
    }
    .background(.quinary)
    .aspectRatio(CGSize(width: 1, height: 1), contentMode: .fit)
    .clipShape(
      .rect(
        topLeadingRadius: 20,
        bottomLeadingRadius: 20,
        bottomTrailingRadius: 20,
        topTrailingRadius: 20
      )
    )
    .padding([.horizontal])
    .gesture(
      DragGesture(minimumDistance: 0, coordinateSpace: .local)
        .onChanged { value in
          if handwritingState.currentPath.isEmpty {
            handwritingState.currentPath.move(to: value.startLocation)
            handwritingState.startLocation = value.startLocation
          } else {
            handwritingState.currentPath.addLine(to: value.location)
          }

          handwritingState.points.append(
            StrokePoint.init(
              x: Float(value.location.x),
              y: Float(value.location.y),
              t: Int(Date.now.timeIntervalSince1970 * 1000)))
        }
        .onEnded { value in
          if CGPointDistanceSquared(from: handwritingState.startLocation, to: value.location) ?? 0
            < (epsilon * epsilon)
          {
            handwritingState.currentPath.addEllipse(
              in: CGRect(x: value.location.x - 3, y: value.location.y - 3, width: 6, height: 6))
            handwritingState.paths.append((path: handwritingState.currentPath, isFilled: true))
          } else {
            handwritingState.paths.append((path: handwritingState.currentPath, isFilled: false))
          }

          handwritingState.strokes.append(Stroke.init(points: handwritingState.points))
          handwritingState.points = []
          handwritingState.currentPath = Path()
          handwritingState.startLocation = nil

          handwritingState.doRecognition()
        }
    )
  }

  // MARK: - Candidates
  @ViewBuilder
  func CandidatesView() -> some View {
    ScrollView(.horizontal) {
      HStack {
        ForEach((0...4), id: \.self) { index in
          if handwritingState.candidates.count > index {
            Button {
              searchText.append(handwritingState.candidates[index])
              buttonText = "Search for: \(searchText)"
              handwritingState.clear()
            } label: {
              Text(handwritingState.candidates[index])
                .lineLimit(1)
                .truncationMode(.middle)
            }
            .font(.title)
            .padding(.horizontal)
            .glassEffect()
          }
        }
      }
    }
    .scrollIndicators(.hidden)
    .padding()
  }

  // MARK: - Buttons
  @ViewBuilder
  func ButtonView() -> some View {
    Button {
      handwritingState.clear()
    } label: {
      Text("Reset canvas")
        .lineLimit(1)
        .truncationMode(.middle)
        .frame(maxWidth: .infinity)
    }
    .buttonStyle(.bordered)
    .buttonBorderShape(.capsule)
    .controlSize(.extraLarge)
    .padding(.horizontal)
    .disabled(handwritingState.paths.isEmpty)

    Button {
      searchText = String(searchText.dropLast())
      if searchText.isEmpty {
        buttonText = "Search"
      } else {
        buttonText = "Search for: \(searchText)"
      }
    } label: {
      Text("Backspace")
        .frame(maxWidth: .infinity)
    }
    .buttonStyle(.bordered)
    .buttonBorderShape(.capsule)
    .controlSize(.extraLarge)
    .padding(.horizontal)
    .disabled(searchText == "")

    Button {
      if !searchText.isEmpty {
        searchContext.searchText = searchText
        searchContext.selectedOption = .autoDetect
        isSearchActive = true
      }
      dismiss()
    } label: {
      Text(buttonText)
        .lineLimit(1)
        .truncationMode(.middle)
        .frame(maxWidth: .infinity)
        .contentTransition(.opacity)
        .animation(.easeInOut(duration: 0.3), value: buttonText)
    }
    .buttonStyle(.borderedProminent)
    .buttonBorderShape(.capsule)
    .controlSize(.extraLarge)
    .padding(.horizontal)
    .disabled(searchText == "")
  }
}

@Observable @MainActor final class HandwritingState {
  // For drawing on canvas
  var currentPath: Path = Path()
  var startLocation: CGPoint?
  var paths: [(path: Path, isFilled: Bool)] = []

  // For MLKit
  var recognizer: DigitalInkRecognizer? = nil
  var strokes: [Stroke] = []
  var points: [StrokePoint] = []

  // For character display
  var candidates: [String] = []

  func doRecognition() {
    let ink = Ink.init(strokes: strokes)
    recognizer?.recognize(
      ink: ink,
      completion: { (result: DigitalInkRecognitionResult?, error: Error?) in
        guard let result else { return }
        let texts = result.candidates.map(\.text)
        Task { @MainActor in
          self.candidates = texts
        }
      }
    )
  }

  func clear() {
    currentPath = Path()
    startLocation = nil
    paths = []

    strokes = []
    points = []

    candidates = []
  }
}
