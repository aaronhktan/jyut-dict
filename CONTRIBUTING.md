# Welcome to the Jyut Dictionary contributing guide!

Read our [Code of Conduct](./CODE_OF_CONDUCT.md) to keep our community approachable and respectable.

In this guide you will get an overview of the contribution workflow from opening an issue, creating a PR, reviewing, and merging the PR.

## Issues

If you find a bug, have a feature suggestion, or want to see a new source added, search if an issue already exists. If a related issue doesn't exist, you can open a new issue using a relevant [issue form](https://github.com/aaronhktan/jyut-dict/issues/new/choose). Try sticking to the templates provided, unless the issue truly does not fit under any of the categories—in which case the blank issue can be used.

Make sure to add the appropriate label to your issue!

## Make changes

Instructions for building and running the application are located in the [README](./README.md), at the root of this repository.

When creating a new branch, name it using the following pattern: \<label\>/\<username\>/\<appropriate branch name\>. Generally, labels fall into the following categories: bugfix, feature, source, or maintenance. For example, a branch for a bugfix created by user aaronhktan for colour issues might be named `bugfix/aaronhktan/colour-issues`.

In general, you should test your changes on the latest release of macOS, Windows, and Ubuntu. Testing on all three would be best, but if that's not possible for you, make sure to get somebody to do it when you make your PR!

## Pull Request

Once you've made some changes and are ready to get it into the main codebase, create a pull request! Follow the pull request template as best as possible. In general, PRs should be titled using the imperative conjugation, e.g. `#<issue name>: Fix colours in macOS`.

After getting the PR reviewed and approved, submit it! Use a squashed merge, with the commit message as `#<issue number>: <short description>`.

## Merged!

Thank you for your contribution!
