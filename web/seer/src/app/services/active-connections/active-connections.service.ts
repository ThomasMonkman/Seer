import { Injectable } from '@angular/core';

import { Observable } from 'rxjs/Observable';
import { ReplaySubject } from 'rxjs/ReplaySubject';
import { ConnectionMeta } from 'app/classes/connnection-meta';
import { LocalStorageService } from 'angular-2-local-storage';

@Injectable()
export class ActiveConnectionsService {
  private readonly storageKey: string = 'activeConnections';
  private connections: ConnectionMeta[];
  private readonly connectionsSubject: ReplaySubject<ConnectionMeta[]> = new ReplaySubject(1);
  constructor(private localStorageService: LocalStorageService) {    
    const connectionInStorage = this.localStorageService.get<string>(this.storageKey);
    this.connections = connectionInStorage === null ? [] : JSON.parse(connectionInStorage);
    this.connectionsSubject.next(this.connections);
  }

  public get(): Observable<ConnectionMeta[]> {
    return this.connectionsSubject.asObservable();
  }

  public store(connection: ConnectionMeta) {
    this.connections.push(connection);
    this.localStorageService.set(this.storageKey, JSON.stringify(this.connections));
    this.connectionsSubject.next(this.connections);
  }

  public remove(name: string) {
    this.connections = this.connections.filter(connection => connection.name === name);
    this.localStorageService.set(this.storageKey, JSON.stringify(this.connections));
    this.connectionsSubject.next(this.connections);
  }
}
