
## კომპილაცია
```sh
make
```

## ტესტებისთვის საჭირო data ფაილები
`make` ის პირველი გაშვება ავტომატურად შექმნის data დირექტორიას ტესტებისთვის საჭირო ფაილებით.  
თუ რატომღაც ეს ფაილები "დაგიზიანდათ", მათი თავიდან ჩამოტვირთვისთვის გაუშვით:
```sh
rm -rf data/
make data
```

## გაშვება
```sh
./six-degrees
```

## მეხსიერებაზე შემოწმება
```sh
./six-degrees-checker64 ./six-degrees
# check for memory leaks
./six-degrees-checker64 ./six-degrees -m
```

