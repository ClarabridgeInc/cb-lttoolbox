# Clarabridge fork of [lttoolbox](https://github.com/apertium/lttoolbox)

## Publishing

We use [reckon] (https://github.com/ajoberstar/reckon) gradle plugin for publishing

There are appropriate samples listed at one of the [wiki] (https://github.com/ajoberstar/reckon/blob/master/docs/index.md)

In order to publish a major release, use:
* $ ./gradlew build reckonTagPush -Preckon.scope=major -Preckon.stage=final 

For a minor one:
* $ ./gradlew build reckonTagPush -Preckon.scope=minor -Preckon.stage=final 
