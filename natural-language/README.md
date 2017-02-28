# NLP

## Anagrams

Run this on the collected works of [Shakespeare] to get all the anagrams.

```
./anagrams shakespeare.txt
```
With the output from that, we can compose some interesting anagrammatic
prose: 

_Silent tinsel. It is tedious outside. We must erect Crete.
Even trouts have tutors. Thou hast hats and a ragged dagger. A hardy
hydra and the happiest of epitaphs. Not to mention elbow bowel. Pistol
pilots and ghost goths. The shape of heaps. A soothing shooting 
with a hint of thin._

[Shakespeare]: http://www.gutenberg.org/cache/epub/100/pg100.txt

### markov text

Command-line nonsense generating utility. It uses a trie-based
character-level Markov algorithm with some added practical features.
Run ```markov``` on the command-line to see the help message. 

With this handy utility, you can combine as many texts as you like.
For example, [H.P. Lovecraft] with [Friedrich Nietzsche]:

```
$ ./markov -n 8 -l 3000 lovecraft.txt nietzsche.txt

... When it disgusted with the grimace of a devil, and joy and woe,
and I and thou-colred vapours did they strangle you, ye singing
birds of my hopes...
```

This would generate about 3000 characters of nicely formatted
nonsense based on the given text files with an N-gram size of 8. 
If you  don't supply ```-n``` or ```-l``` they default to 4 and 1000
respectively. Varying the N-gram size gives different effects.
For example: ```markov -n 12 lovecraft.txt``` gave

```
Of the living-room I traced a full night a faint, obscure 
telepathic wave of almost fifteen feet each way-sides, 
floor, and seemed to fill up your goblets and passages 
of poor Rhoby Harris's ravings, where shall be the fruit 
was growing up uncannily shrivelled, and the pervasive 
half-impressions not so much hard laboured strokes of 
those on the related some desolate valley, and wondered 
how ever the problematical attempt an attack by the 
jester's whim.

My own family history, was a highly impressive age and 
workmanship of things that are spawned in dead cities 
toward the revivification of markedly unsanitary qualities, 
remoteness of the solstice and of the ...
```

which sounds just like Lovecraft, while ```markov lovecraft.txt``` gave

```
He in blazing the rat, and the mould nor to like a sume wishe the law coulder 
neartantalks waken tal better by footlan-experhapses, toilery prings rectantas 
matione mout what set although and occust shes fields two formularges, withing 
the leging rumbed ...
```

which also sounds like Lovecraft, in a sense.

[H.P. Lovecraft]: https://github.com/nathanielksmith/lovecraftcorpus
[Friedrich Nietzsche]: http://www.gutenberg.org/cache/epub/1998/pg1998.txt

